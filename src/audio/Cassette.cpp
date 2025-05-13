#include "./Cassette.h"

typedef float SAMPLE;

using namespace caf;

namespace Gj {
namespace Audio {

jack_port_t* outPortL;
jack_port_t* outPortR;

constexpr size_t EffectsSettings_RB_SIZE = 2 * MAX_EFFECTS_CHANNELS * sizeof(float);

Cassette::Cassette(actor_system& actorSystem, AppState* gAppState, Mixer* mixer)
  : actorSystem(actorSystem)
  , threadId(ThreadStatics::incrThreadId())
  , fileName(ThreadStatics::getFilePath())
  , gAppState(gAppState)
  , mixer(mixer)
  , jackClient(mixer->getJackClient())
  , sfInfo()
  , effectsChannelsWriteOutBuffer(nullptr)
  , audioData(
      ThreadStatics::getFrameId(),
      PLAY,
      ThreadStatics::getPlaybackSpeed(),
      static_cast<float>(mixer->getEffectsChannelsCount()),
      mixer->getEffectsChannelsCount()
    )
  {

  if (jackClient == nullptr) {
    Logging::write(
      Error,
      "Audio::Cassette::Cassette",
      "jackClient is null"
    );
    throw std::runtime_error(
      "Unable to instantiate Cassette - null jackClient"
    );
  }

  float* buffersAPtrPre[2] = { &buffersA[0], &buffersA[MAX_AUDIO_FRAMES_PER_BUFFER] };
  buffersAPtr = buffersAPtrPre;

  float* buffersBPtrPre[2] = { &buffersB[0], &buffersB[MAX_AUDIO_FRAMES_PER_BUFFER] };
  buffersBPtr = buffersBPtrPre;

  int audioDataRes = setupAudioData();
  if (audioDataRes > 0) {
    Logging::write(
      Error,
      "Audio::Cassette::Cassette",
      "Unable to setup AudioData - status: " + std::to_string(audioDataRes)
    );
    throw std::runtime_error(
      "Unable to instantiate Cassette - AudioData status: " + std::to_string(audioDataRes)
    );
  }

  Logging::write(
    Info,
    "Audio::Cassette::Cassette",
    "Initialized AudioData with index: " + std::to_string(audioData.index)
  );

  if (
    const int setupJackStatus = setupJack();
    setupJackStatus == 0
  ) {
    Logging::write(
      Info,
      "Audio::Cassette::Cassette",
      "Setup Jack"
    );
  } else {
    Logging::write(
      Error,
      "Audio::Cassette::Cassette",
      "Unable to setup Jack - status: " + std::to_string(setupJackStatus)
    );
    throw std::runtime_error(
      "Unable to instantiate Cassette - Jack status: " + std::to_string(setupJackStatus)
    );
  }
}

Cassette::~Cassette() {
  Logging::write(
    Info,
    "Audio::Cassette::~Cassette",
    "Destroying Cassette"
  );

  cleanup();

  Logging::write(
    Info,
    "Audio::Cassette::~Cassette",
    "Destroyed Cassette"
  );
}

void Cassette::cleanup() {
  Logging::write(
    Info,
    "Audio::Cassette::cleanup",
    "Freeing resources for file " + std::string(fileName)
  );

  if (jackClientIsActive) {
    if (jack_deactivate(jackClient)) {
      std::cerr << "Unable to deactivate jack client" << std::endl;
      Logging::write(
        Error,
        "Audio::Cassette::cleanup",
        "Unable to deactivate JackClient"
      );
    };
  }
  jackClientIsActive = false;

  Logging::write(
    Info,
    "Audio::Cassette::cleanup",
    "Deactivated JackClient"
  );

  if (!deleteBuffers()) {
    Logging::write(
      Error,
      "Audio::Cassette::cleanup",
      "Unable to free buffers"
    );
  }
  Logging::write(
    Info,
    "Audio::Cassette::cleanup",
    "Deleted buffers"
  );
  sf_close(file);
  Logging::write(
    Info,
    "Audio::Cassette::cleanup",
    "Done freeing resources for file" + std::string(fileName)
  );
};

// jack process callback
// do not allocate/free memory within this method
// as it may be called at system-interrupt level
int Cassette::jackProcessCallback(jack_nframes_t nframes, void* arg) {
  // get port buffers
  auto* outL = static_cast<jack_default_audio_sample_t*>(
    jack_port_get_buffer(outPortL, nframes)
  );
  auto* outR = static_cast<jack_default_audio_sample_t*>(
    jack_port_get_buffer(outPortR, nframes)
  );

  // retrieve AudioData
  const auto audioData = static_cast<AudioData*>(arg);
  const sf_count_t audioDataIndex = audioData->index;

  // update process head
  audioData->inputBuffersProcessHead[0] = audioData->inputBuffers[0] + audioDataIndex;
  audioData->inputBuffersProcessHead[1] = audioData->inputBuffers[1] + audioDataIndex;
  float** processHead = audioData->inputBuffersProcessHead;

  // process effects channels
  // main channel is effectsChannelIdx 0
  for (int effectsChannelIdx = 1; effectsChannelIdx < audioData->effectsChannelCount + 1; effectsChannelIdx++) {
    auto [effectCount, processFuncs, buffers] = audioData->effectsChannelsProcessData[effectsChannelIdx];
    for (int pluginIdx = 0; pluginIdx < effectCount; pluginIdx++) {
      if (pluginIdx == 0) {
        buffers[pluginIdx].inputs = processHead;
      }
      buffers[pluginIdx].numSamples = nframes;

      processFuncs[pluginIdx](
        buffers[pluginIdx],
        nframes
      );
    }
  }

  // read channel settings from ringbuffer
  jack_ringbuffer_t* ringBuffer = audioData->effectsChannelsSettingsRB;
  if (jack_ringbuffer_read_space(ringBuffer) >= EffectsSettings_RB_SIZE) {
    jack_ringbuffer_read(ringBuffer, reinterpret_cast<char*>(audioData->effectsChannelsSettings), EffectsSettings_RB_SIZE);
  }

  // sum down into mainInBuffers
  for (int i = 0; i < nframes; i++) {
    for (int effectsChannelIdx = 1; effectsChannelIdx < audioData->effectsChannelCount + 1; effectsChannelIdx++) {
      const float factorLL = audioData->effectsChannelsSettings[4 * effectsChannelIdx];
      const float factorLR = audioData->effectsChannelsSettings[4 * effectsChannelIdx + 1];
      const float factorRL = audioData->effectsChannelsSettings[4 * effectsChannelIdx + 2];
      const float factorRR = audioData->effectsChannelsSettings[4 * effectsChannelIdx + 3];

      if (effectsChannelIdx == 1) {
        if (audioData->effectsChannelsProcessData[effectsChannelIdx].effectCount == 0) {
          audioData->mainInBuffers[0][i] = factorLL * processHead[0][i] + factorRL * processHead[1][i];
          audioData->mainInBuffers[1][i] = factorLR * processHead[0][i] + factorRR * processHead[1][i];
        } else {
          audioData->mainInBuffers[0][i] = factorLL * audioData->effectsChannelsWriteOut[effectsChannelIdx][0][i] + factorRL * audioData->effectsChannelsWriteOut[effectsChannelIdx][1][i];
          audioData->mainInBuffers[1][i] = factorLR * audioData->effectsChannelsWriteOut[effectsChannelIdx][0][i] + factorRR * audioData->effectsChannelsWriteOut[effectsChannelIdx][1][i];
        }
      } else {
        if (audioData->effectsChannelsProcessData[effectsChannelIdx].effectCount == 0) {
          audioData->mainInBuffers[0][i] += factorLL * processHead[0][i] + factorRL * processHead[1][i];
          audioData->mainInBuffers[1][i] += factorLR * processHead[0][i] + factorRR * processHead[1][i];
        } else {
          audioData->mainInBuffers[0][i] += factorLL * audioData->effectsChannelsWriteOut[effectsChannelIdx][0][i] + factorRL * audioData->effectsChannelsWriteOut[effectsChannelIdx][1][i];
          audioData->mainInBuffers[1][i] += factorLR * audioData->effectsChannelsWriteOut[effectsChannelIdx][0][i] + factorRR * audioData->effectsChannelsWriteOut[effectsChannelIdx][1][i];
        }
      }
    }
  }

  // process summed down mix through main effects
  auto [effectCount, processFuncs, buffers] = audioData->effectsChannelsProcessData[0];
  for (int pluginIdx = 0; pluginIdx < effectCount; pluginIdx++) {
    buffers[pluginIdx].numSamples = nframes;

    processFuncs[pluginIdx](
      buffers[pluginIdx],
      nframes
    );
  }

  // write out processed main channel to audio out
  const float factorLL = audioData->effectsChannelsSettings[0];
  const float factorLR = audioData->effectsChannelsSettings[1];
  const float factorRL = audioData->effectsChannelsSettings[2];
  const float factorRR = audioData->effectsChannelsSettings[3];
  for (int i = 0; i < nframes; i++) {
    outL[i] = factorLL * audioData->mainOutBuffers[0][i] + factorRL * audioData->mainOutBuffers[1][i];
    outR[i] = factorLR * audioData->mainOutBuffers[0][i] + factorRR * audioData->mainOutBuffers[1][i];
  }

  audioData->index += nframes;
  return 0;
}

int Cassette::setupAudioData() {
  Logging::write(
    Info,
    "Audio::Cassette::setupAudioData",
    "Setting up Cassette AudioData"
  );

  // https://svn.ict.usc.edu/svn_vh_public/trunk/lib/vhcl/libsndfile/doc/api.html
  // > When opening a file for read, the format field should be set to zero before calling sf_open().
  sfInfo.format = 0;
  file = sf_open(fileName, SFM_READ, &sfInfo);
  if (file == nullptr) {
    Logging::write(
      Error,
      "Audio::Cassette::setupAudioData",
      "Unable to open input file : sf : " + std::string(sf_strerror(nullptr))
    );
    return 1;
  };

  // update plugin effects with info about audio to be processed
  if (!mixer->setSampleRate(sfInfo.samplerate)) {
    Logging::write(
      Error,
      "Audio::Cassette::setupAudioData",
      "Unable to set sample rate: " + std::to_string(sfInfo.samplerate)
    );
  }

  // Allocate memory for data
  buffer = new float[sfInfo.frames * sfInfo.channels];
  if (!buffer) {
      Logging::write(
        Error,
        "Audio::Cassette::setupAudioData",
        "Cannot allocate memory for raw audio buffer"
      );
    return 2;
  }

  // Read the audio data into buffer
  long readcount = sf_read_float(file, buffer, sfInfo.frames * sfInfo.channels);
  if (readcount == 0) {
    Logging::write(
      Error,
      "Audio::Cassette::setupAudioData",
      "Unable to read file: " + std::string(fileName)
    );
    return 3;
  }

  if (!setupInputBuffers()) {
    Logging::write(
      Error,
      "Audio::Cassette::setupAudioData",
      "Unable to setup inputBuffers"
    );
    return 4;
  };

  if (!allocateProcessBuffers()) {
    Logging::write(
      Error,
      "Audio::Cassette::setupAudioData",
      "Unable to allocate processBuffers"
    );
    return 4;
  }

  Logging::write(
    Info,
    "Audio::Cassette::setupAudioData",
    "Allocated process buffers"
  );

  Logging::write(
    Info,
    "Audio::Cassette::setupAudioData",
    "Setting up AudioData buffers"
  );

  audioData.inputBuffers[0] = inputBuffers[0];
  audioData.inputBuffers[1] = inputBuffers[1];

  audioData.inputBuffersProcessHead[0] = inputBuffers[0] + audioData.index;
  audioData.inputBuffersProcessHead[1] = inputBuffers[1] + audioData.index;

  audioData.mainInBuffers[0] = buffersAPtr[0];
  audioData.mainInBuffers[1] = buffersAPtr[1];

  for (int i = 0; i < MAX_EFFECTS_CHANNELS; i++) {
    audioData.effectsChannelsWriteOut[i][0] = effectsChannelsWriteOutBuffer + (2 * i * MAX_AUDIO_FRAMES_PER_BUFFER);
    audioData.effectsChannelsWriteOut[i][1] = effectsChannelsWriteOutBuffer + ((2 * i + 1) * MAX_AUDIO_FRAMES_PER_BUFFER);
  }

  Logging::write(
    Info,
    "Audio::Cassette::setupAudioData",
    "Successfully setup AudioData buffers."
  );

  // populate initial channel settings
  for (int i = 0; i < mixer->getEffectsChannelsCount() + 1; i++) {
    effectsSettingsBuffer[4 * i]     = mixer->getEffectsChannel(i)->getGain();
    effectsSettingsBuffer[4 * i + 1] = mixer->getEffectsChannel(i)->getMute();
    effectsSettingsBuffer[4 * i + 2] = mixer->getEffectsChannel(i)->getSolo();
    effectsSettingsBuffer[4 * i + 3] = mixer->getEffectsChannel(i)->getPan();
  }

  Logging::write(
    Info,
    "Audio::Cassette::setupAudioData",
    "Populated initial effectsSettings into buffer"
  );

  Logging::write(
    Info,
    "Audio::Cassette::setupAudioData",
    "Setting up AudioData effects processing..."
  );

  for (const auto effectsChannel : mixer->getEffectsChannels()) {
    const auto effectsChannelIdx = effectsChannel->getIndex();
    audioData.effectsChannelsProcessData[effectsChannelIdx].effectCount = effectsChannel->effectCount();
    audioData.effectsChannelsSettings[2 * effectsChannelIdx] = effectsChannel->getGain();
    audioData.effectsChannelsSettings[2 * effectsChannelIdx + 1] = effectsChannel->getPan();

    if (effectsChannelIdx == 0) {
      if (audioData.effectsChannelsProcessData[0].effectCount % 2 == 0) {
        audioData.mainOutBuffers[0] = buffersAPtr[0];
        audioData.mainOutBuffers[1] = buffersAPtr[1];
      } else {
        // effectCount % 2 == 1
        audioData.mainOutBuffers[0] = buffersBPtr[0];
        audioData.mainOutBuffers[1] = buffersBPtr[1];
      }
    }

    for (int pluginIdx = 0; pluginIdx < effectsChannel->effectCount(); pluginIdx++) {
      const auto plugin = effectsChannel->getPluginAtIdx(pluginIdx);
      audioData.effectsChannelsProcessData[effectsChannelIdx].processFuncs[pluginIdx] =
        [ObjectPtr = plugin->audioHost->audioClient](auto && PH1, auto && PH2) { return ObjectPtr->process(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2)); };

      if (effectsChannelIdx == 0) { // main channel
        audioData.effectsChannelsProcessData[0].buffers[pluginIdx] = getPluginBuffersMain(pluginIdx);
      } else {
        audioData.effectsChannelsProcessData[effectsChannelIdx].buffers[pluginIdx] = getPluginBuffers(effectsChannel, effectsChannelIdx, pluginIdx, audioData);
      }
    }
  }

  Logging::write(
    Info,
    "Audio::Cassette::setupAudioData",
    "Setup AudioData Effects processing."
  );

  Logging::write(
    Info,
    "Audio::Cassette::setupAudioData",
    "Successfully setup AudioData."
  );

  return 0;
}

IAudioClient::Buffers Cassette::getPluginBuffersMain(const int pluginIdx) const {
  const int32_t audioFramesPerBuffer = gAppState->audioFramesPerBuffer;
  if (pluginIdx % 2 == 0) {
    const IAudioClient::Buffers buffers = {
      buffersAPtr,
      2,
      buffersBPtr,
      2,
      audioFramesPerBuffer
    };
    return buffers;
  }

  // pluginIdx % 2 == 1
  const IAudioClient::Buffers buffers = {
    buffersBPtr,
    2,
    buffersAPtr,
    2,
    audioFramesPerBuffer
  };
  return buffers;
}

IAudioClient::Buffers Cassette::getPluginBuffers(const Effects::EffectsChannel* effectsChannel, const int channelIdx, const int pluginIdx, const AudioData& audioData) const {
  const int32_t audioFramesPerBuffer = gAppState->audioFramesPerBuffer;
  const int effectsCount = effectsChannel->effectCount(); // should always be >0
  if (pluginIdx % 2 == 0) {
    if (pluginIdx == effectsCount - 1) {
      const auto writeOut = const_cast<float**>(audioData.effectsChannelsWriteOut[channelIdx]);
      const IAudioClient::Buffers buffers = {
        buffersBPtr, // if pluginIdx == 0, this will be updated to processHead in jackProcessCallBAck
        2,
        writeOut,
        2,
        audioFramesPerBuffer
      };
      return buffers;
    }

    const IAudioClient::Buffers buffers = {
      buffersBPtr,
      2,
      buffersAPtr,
      2,
      audioFramesPerBuffer
    };
    return buffers;
  }

  // pluginIdx % 2 == 1
  if (pluginIdx == effectsCount - 1) {
    const auto writeOut = const_cast<float**>(audioData.effectsChannelsWriteOut[channelIdx]);
    const IAudioClient::Buffers buffers = {
      buffersAPtr,
      2,
      writeOut,
      2,
      audioFramesPerBuffer
    };
    return buffers;
  }

  const IAudioClient::Buffers buffers = {
    buffersAPtr,
    2,
    buffersBPtr,
    2,
    audioFramesPerBuffer
  };
  return buffers;
}

int Cassette::setupJack() {
  const int setProcessStatus = jack_set_process_callback(
    jackClient,
    &Cassette::jackProcessCallback,
    &audioData
  );
  if ( setProcessStatus == 0) {
    Logging::write(
      Info,
      "Audio::Cassette::setupJack",
      "Set Jack process callback"
    );
  } else {
    Logging::write(
      Error,
      "Audio::Cassette::setupJack",
      "Unable to set process callback - status: " + std::to_string(setProcessStatus)
    );
    return 1;
  }

  if (outPortL == nullptr) {
      outPortL = jack_port_register(
        jackClient,
        "out_port_L",
        JACK_DEFAULT_AUDIO_TYPE,
        JackPortIsOutput,
        0
      );
  }

  if (outPortL == nullptr) {
    Logging::write(
      Error,
      "Audio::Cassette::setupJack",
      "Unable to create Jack outPortL"
    );
    return 2;
  }

  if (outPortR == nullptr) {
    outPortR = jack_port_register(
      jackClient,
      "out_port_R",
      JACK_DEFAULT_AUDIO_TYPE,
      JackPortIsOutput,
      0
    );
  }
  if (outPortR == nullptr) {
    Logging::write(
      Error,
      "Audio::Cassette::setupJack",
      "Unable to create Jack outPortR"
    );
    return 3;
  }

  int jackActivateStatus = jack_activate(jackClient);
  jackClientIsActive = true;
  if (jackActivateStatus == 0) {
    Logging::write(
      Info,
      "Audio::Cassette::setupJack",
      "Jack activated successfully"
    );
  } else {
    Logging::write(
      Error,
      "Audio::Cassette::setupJack",
      "Unable to activate jack - status: " + std::to_string(jackActivateStatus)
    );
    return 4;
  }

  if (
    const int connectStatusL = jack_connect(jackClient, jack_port_name(outPortL), "system:playback_1");
    connectStatusL != 0
  ) {
    Logging::write(
      Error,
      "Audio::Cassette::setupJack",
      "Unable to connect out_port_L - status: " + std::to_string(connectStatusL)
    );
    return 5;
  }

  if (
    const int connectStatusR = jack_connect(jackClient, jack_port_name(outPortR), "system:playback_2");
    connectStatusR != 0
  ) {
      Logging::write(
        Error,
        "Audio::Cassette::setupJack",
        "Unable to connect out_port_R - status: " + std::to_string(connectStatusR)
      );
      return 6;
  }

  // TESTING SHOW PORTS
  // const char** ports = jack_get_ports(jackClient, nullptr, nullptr, 0);
  // if (ports == nullptr) {
  //   printf("No ports found.\n");
  // } else {
  //   for (int i = 0; ports[i] != nullptr; i++) {
  //     printf("Port: %s\n", ports[i]);
  //   }
  // }
  // TESTING SHOW PORTS

  return 0;
}

bool Cassette::allocateProcessBuffers() {
  Logging::write(
    Info,
    "Audio::Cassette::allocateProcessBuffers",
    "Allocating process buffers."
  );

  effectsChannelsWriteOutBuffer = new float[2 * MAX_AUDIO_FRAMES_PER_BUFFER * MAX_EFFECTS_CHANNELS];

  Logging::write(
    Info,
    "Audio::Cassette::allocateProcessBuffers",
    "Allocated process buffers."
  );

  return true;
}

bool Cassette::allocateInputBuffers() {
  // left and right channels held in contiguous memory
  inputBuffer = new float[2 * sfInfo.frames];
  inputBuffers[0] = inputBuffer;
  inputBuffers[1] = inputBuffer + sfInfo.frames;

  if (inputBuffers[0] == nullptr || inputBuffers[1] == nullptr) {
    Logging::write(
      Error,
      "Audio::Cassette::allocateInputBuffers",
      "Unable to allocate memory for individual Cassette.inputBuffers"
    );
    return false;
  }

  return true;
}

bool Cassette::populateInputBuffers() const {
  if (buffer == nullptr) {
    Logging::write(
      Error,
      "Audio::Cassette::populateInputBuffers",
      "Unable to populate input buffers - raw audio buffer is null"
    );
    return false;
  }

  if (inputBuffers[0] == nullptr || inputBuffers[1] == nullptr) {
    Logging::write(
      Error,
      "Audio::Cassette::populateInputBuffers",
      "Unable to populate input buffers - buffers are null"
    );
    return false;
  }

  // de-interlace audio into shared input buffers
  for (int i = 0; i < sfInfo.frames / 2; i++) {
    inputBuffers[0][i] = buffer[2 * i];
    inputBuffers[1][i] = buffer[2 * i + 1];
  }

  return true;
}

bool Cassette::setupInputBuffers() {
  if (!allocateInputBuffers()) {
    Logging::write(
      Error,
      "Audio::Cassette::setupInputBuffers",
      "Unable to allocate input buffers."
    );
  }

  if (!populateInputBuffers()) {
    Logging::write(
      Error,
      "Audio::Cassette::setupInputBuffers",
      "Unable to populate input buffers."
    );
    return false;
  };

  return true;
}

bool Cassette::deleteBuffers() const {
  Logging::write(
    Info,
    "Audio::Cassette::deleteBuffers",
    "Deleting Cassette buffers."
  );

  try {
    delete[] buffer;
    delete[] inputBuffer;
    delete[] effectsChannelsWriteOutBuffer;
  } catch (...) {
    Logging::write(
        Error,
        "Audio::Cassette::freeBuffers",
        "Unable to delete Cassette buffers"
    );
    return false;
  }

  Logging::write(
    Info,
    "Audio::Cassette::freeBuffers",
    "Done delete Cassette buffers."
  );

  return true;
}

int Cassette::updateAudioDataFromMixer(jack_ringbuffer_t* effectsChannelsSettingsRB, const int channelCount) {
  const float channelCountF = static_cast<float>(channelCount);

  float soloVals[MAX_EFFECTS_CHANNELS] {0.0f};
  float soloLVals[MAX_EFFECTS_CHANNELS] {0.0f};
  float soloRVals[MAX_EFFECTS_CHANNELS] {0.0f};
  bool soloEngaged = false;
  for (int i = 1; i < channelCount; i++) {
    soloVals[i] = mixer->getEffectsChannel(i)->getSolo();
    soloLVals[i] = mixer->getEffectsChannel(i)->getSoloL();
    soloRVals[i] = mixer->getEffectsChannel(i)->getSoloR();
    if (soloVals[i] == 1.0f || soloLVals[i] == 1.0f || soloRVals[i] == 1.0f)
      soloEngaged = true;
  }

  // update buffer
  for (int i = 0; i < channelCount; i++) {
    const auto effectsChannel = mixer->getEffectsChannel(i);
    const float gain = effectsChannel->getGain();
    const float gainL = effectsChannel->getGainL();
    const float gainR = effectsChannel->getGainR();
    const float mute = effectsChannel->getMute();
    const float muteL = effectsChannel->getMuteL();
    const float muteR = effectsChannel->getMuteR();
    const float solo = i == 0 // no solo on main
                        ? 1.0f
                        : soloEngaged ? soloVals[i] : 1.0f;
    const float soloL = soloEngaged ? soloLVals[i] : 1.0f;
    const float soloR = soloEngaged ? soloRVals[i] : 1.0f;
    const float pan  = effectsChannel->getPan();
    const float panL = effectsChannel->getPanL();
    const float panR = effectsChannel->getPanR();

    effectsSettingsBuffer[4 * i] = AudioData::factorLL(
      gain, gainL, gainR,
      mute, muteL, muteR,
      solo, soloL, soloR,
      pan, panL, panR,
      channelCountF
    );
    effectsSettingsBuffer[4 * i + 1] = AudioData::factorLR(
      gain, gainL, gainR,
      mute, muteL, muteR,
      solo, soloL, soloR,
      pan, panL, panR,
      channelCountF
    );
    effectsSettingsBuffer[4 * i + 2] = AudioData::factorRL(
      gain, gainL, gainR,
      mute, muteL, muteR,
      solo, soloL, soloR,
      pan, panL, panR,
      channelCountF
    );
    effectsSettingsBuffer[4 * i + 3] = AudioData::factorRR(
      gain, gainL, gainR,
      mute, muteL, muteR,
      solo, soloL, soloR,
      pan, panL, panR,
      channelCountF
    );
  }

  // write to ring buffer
  if (jack_ringbuffer_write_space(effectsChannelsSettingsRB) >= EffectsSettings_RB_SIZE) {
    jack_ringbuffer_write(effectsChannelsSettingsRB, reinterpret_cast<char*>(effectsSettingsBuffer), EffectsSettings_RB_SIZE);
  }

  return 0;
}

  // TODO:
  // - cleanup when shutdown during playback
int Cassette::play() {
  Logging::write(
    Info,
    "Audio::Cassette::play",
    "Playing Cassette..."
  );

  const int channelCount = mixer->getEffectsChannelsCount() + 1;

  jack_ringbuffer_t* effectsChannelsSettingsRB = jack_ringbuffer_create(EffectsSettings_RB_SIZE);
  audioData.effectsChannelsSettingsRB = effectsChannelsSettingsRB;

  while(
          audioData.playState != STOP
            && audioData.playState != PAUSE
            && audioData.index > -1
            && audioData.index < sfInfo.frames
  ) {
    // hold thread open until stopped

    // here is our chance to pull data out of the application
    // and
    // make it accessible to our running audio callback through the audioData obj

    if (audioData.readComplete) { // reached end of input file
        ThreadStatics::setPlayState(STOP);
        ThreadStatics::setReadComplete(true);
        Logging::write(
          Info,
          "Audio::Cassette::play",
          "Read complete"
        );
        break;
    }

    if (audioData.fadeIn > 0.01) {
        audioData.fadeIn -= 0.01;
        audioData.volume += 0.01;
    }

    updateAudioDataFromMixer(effectsChannelsSettingsRB, channelCount);

    if ( threadId != ThreadStatics::getThreadId() ) { // fadeout, break + cleanup
        if (audioData.fadeOut < 0.01) { // break + cleanup
            break;
        } else { // continue fading out
            audioData.volume -= 0.001;
            audioData.fadeOut -= 0.001;
        }
    } else {
        audioData.playbackSpeed = ThreadStatics::getPlaybackSpeed();
        audioData.playState = ThreadStatics::getPlayState();
        ThreadStatics::setFrameId( audioData.index );
    }

    std::this_thread::sleep_for( std::chrono::milliseconds(10) );
  } // end of while loop

  if ( threadId == ThreadStatics::getThreadId() ) { // current audio thread has reached natural end of file
      if (audioData.playState == PLAY) {
          ThreadStatics::setPlayState(STOP);
      } else {
          ThreadStatics::setPlayState(audioData.playState);
      }
      ThreadStatics::setReadComplete(true);
  }

  if (jackClientIsActive)
    jack_deactivate(jackClient);

  jackClientIsActive = false;

  return 0;
};

} // Audio
} // Gj
