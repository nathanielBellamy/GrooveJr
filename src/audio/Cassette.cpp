#include "./Cassette.h"

typedef float SAMPLE;

using namespace caf;

namespace Gj {
namespace Audio {

jack_port_t* outPortL;
jack_port_t* outPortR;

Cassette::Cassette(actor_system& actorSystem, long threadId, const char* fileName, long initialFrameId, AppState* gAppState, Mixer* mixer)
  : actorSystem(actorSystem)
  , threadId(threadId)
  , fileName(fileName)
  , initialFrameId(initialFrameId)
  , gAppState(gAppState)
  , mixer(mixer)
  , jackClient(mixer->getJackClient())
  , sfInfo()
  {

  if (jackClient == nullptr) {
    Logging::write(
      Error,
      "Cassette::Cassette",
      "jackClient is null"
    );
    throw std::runtime_error(
      "Unable to instantiate Cassette - null jackClient"
    );
  }

  AudioDataResult audioDataRes = setupAudioData();
  if (std::holds_alternative<int>(audioDataRes)) {
    Logging::write(
      Error,
      "Cassette::Cassette",
      "Unable to setup AudioData - status: " + std::to_string(std::get<int>(audioDataRes))
    );
    throw std::runtime_error(
      "Unable to instantiate Cassette - AudioData status: " + std::to_string(std::get<int>(audioDataRes))
    );
  }
  audioData = std::get<AudioData>(audioDataRes);

  if (
    const int setupJackStatus = setupJack();
    setupJackStatus == 0
  ) {
    Logging::write(
      Info,
      "Cassette::Cassette",
      "Setup Jack"
    );
  } else {
    Logging::write(
      Error,
      "Cassette::Cassette",
      "Unable to setup Jack - status: " + std::to_string(setupJackStatus)
    );
    throw std::runtime_error(
      "Unable to instantiate Cassette - Jack status: " + std::to_string(setupJackStatus)
    );
  }
}

Cassette::~Cassette() {
  cleanup();
}

void Cassette::cleanup() const {
  jack_deactivate(jackClient);
  freeBuffers();
  sf_close(file);
  Logging::write(
    Info,
    "Cassette::cleanup",
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

  // process effects
  for (int effectsChannelIdx = 0; effectsChannelIdx < audioData->effectsChannelCount; effectsChannelIdx++) {
    auto [effectCount, processFuncs, buffers, channelSettings] = audioData->effectsChannelsProcessData[effectsChannelIdx];
    if (effectCount > 0) { // update leading buffer reading direct from audio source
      buffers[0].inputs = processHead;
    }
    for (int effectIdx = 0; effectIdx < effectCount; effectIdx++) {
      processFuncs[effectIdx](
        buffers[effectIdx],
        nframes
      );
    }
  }

  // sum down into output buffer
  const float channelCount = audioData->channelCount;
  for (int i = 0; i < nframes; i++) {
    outL[i] = audioData->dryChannel.gain * processHead[0][i] / channelCount;
    outR[i] = audioData->dryChannel.gain * processHead[1][i] / channelCount;

    for (int effectsChannelIdx = 0; effectsChannelIdx < audioData->effectsChannelCount; effectsChannelIdx++) {
      auto channel = audioData->effectsChannelsProcessData[effectsChannelIdx].channelSettings;
      outL[i] += channel.gain * audioData->effectsChannelsWriteOut[effectsChannelIdx][0][i] / channelCount;
      outR[i] += channel.gain * audioData->effectsChannelsWriteOut[effectsChannelIdx][1][i] / channelCount;
    }
  }

  audioData->index += nframes;
  return 0;
}

AudioDataResult Cassette::setupAudioData() {
  // https://svn.ict.usc.edu/svn_vh_public/trunk/lib/vhcl/libsndfile/doc/api.html
  // > When opening a file for read, the format field should be set to zero before calling sf_open().
  sfInfo.format = 0;
  file = sf_open(fileName, SFM_READ, &sfInfo);
  if (file == nullptr) {
    Logging::write(
      Error,
      "Cassette::setupAudioData",
      "Unable to open input file : sf : " + std::string(sf_strerror(nullptr))
    );
    return 1;
  };

  // update plugin effects with info about audio to be processed
  if (!mixer->setSampleRate(sfInfo.samplerate)) {
    Logging::write(
      Error,
      "Cassette::setupAudioData",
      "Unable to set sample rate: " + std::to_string(sfInfo.samplerate)
    );
  }

  // Allocate memory for data
  buffer = static_cast<float *>(malloc(sfInfo.frames * sfInfo.channels * sizeof(float)));
  if (!buffer) {
      Logging::write(
        Error,
        "Cassette::setupAudioData",
        "Cannot allocate memory for audioDataBuffer"
      );
    return 2;
  }

  // Read the audio data into buffer
  long readcount = sf_read_float(file, buffer, sfInfo.frames * sfInfo.channels);
  if (readcount == 0) {
    Logging::write(
      Error,
      "Cassette::setupAudioData",
      "Unable to read file: " + std::string(fileName)
    );
    return 3;
  }

  if (!setupInputBuffers()) {
    Logging::write(
      Error,
      "Cassette::setupAudioData",
      "Unable to setup inputBuffers"
    );
    return 4;
  };

  return AudioData(initialFrameId, PLAY, inputBuffers, buffersA, buffersB, effectsChannelsWriteOutBuffer);
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
      "Cassette::setupJack",
      "Set Jack process callback"
    );
  } else {
    Logging::write(
      Error,
      "Cassette::setupJack",
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
      "Cassette::setupJack",
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
      "Cassette::setupJack",
      "Unable to create Jack outPortR"
    );
    return 3;
  }

  int jackActivateStatus = jack_activate(jackClient);
  if (jackActivateStatus == 0) {
    Logging::write(
      Info,
      "Cassette::setupJack",
      "Jack activated successfully"
    );
  } else {
    Logging::write(
      Error,
      "Cassette::setupJack",
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
      "Cassette::setupJack",
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
        "Cassette::setupJack",
        "Unable to connect out_port_R - status: " + std::to_string(connectStatusR)
      );
      return 6;
  }

  // TESTING SHOW PORTS
  const char** ports = jack_get_ports(jackClient, nullptr, nullptr, 0);
  if (ports == nullptr) {
    printf("No ports found.\n");
  } else {
    for (int i = 0; ports[i] != nullptr; i++) {
      printf("Port: %s\n", ports[i]);
    }
  }
  // TESTING SHOW PORTS

  return 0;
}

bool Cassette::allocateProcessBuffers() {
  buffersA = static_cast<float**>(
  	malloc(2 * gAppState->audioFramesPerBuffer * sizeof(float))
  );
  buffersB = static_cast<float**>(
	malloc(2 * gAppState->audioFramesPerBuffer * sizeof(float))
  );

  if (buffersA == nullptr || buffersB == nullptr) {
    Logging::write(
      Error,
      "Cassette::allocateProcessBuffers",
      "Unable to allocate memory for buffersA or buffersB."
    );
    throw std::runtime_error ("Unable to allocate memory for Cassette buffers.");
  }

  for (int c = 0; c < 2; c++) {
  	buffersA[c] = new float[gAppState->audioFramesPerBuffer];
	  buffersB[c] = new float[gAppState->audioFramesPerBuffer];
  }

  effectsChannelsWriteOutBuffer = static_cast<float***>(
  	malloc(2 * gAppState->audioFramesPerBuffer * sizeof(float) * MAX_EFFECTS_CHANNELS)
  );

  for (int i = 0; i < MAX_EFFECTS_CHANNELS; i++) {
    effectsChannelsWriteOutBuffer[i][0] = new float[gAppState->audioFramesPerBuffer];
    effectsChannelsWriteOutBuffer[i][1] = new float[gAppState->audioFramesPerBuffer];
  }

  return true;
}

bool Cassette::allocateInputBuffers() {
  if (inputBuffers == nullptr) {
    inputBuffers = static_cast<float**>(
        malloc(2 * sfInfo.frames * sizeof(float))
    );
  } else {
    inputBuffers = static_cast<float**>(
      realloc(inputBuffers, 2 * sfInfo.frames * sizeof(float*))
    );
  }

  if (inputBuffers == nullptr) {
    Logging::write(
      Error,
      "Cassette::allocateInputBuffers",
      "Unable to allocate memory for Cassette.inputBuffers"
    );
    return false;
  }

  for (int c = 0; c < 2; c++) {
    inputBuffers[c] = new float[sfInfo.frames];
  }

  if (inputBuffers[0] == nullptr || inputBuffers[1] == nullptr) {
    Logging::write(
      Error,
      "Cassette::allocateInputBuffers",
      "Unable to allocate memory for individual Cassette.inputBuffers"
    );
    return false;
  }

  return true;
}

bool Cassette::populateInputBuffers() const {
  if (inputBuffers == nullptr || inputBuffers[0] == nullptr || inputBuffers[1] == nullptr) {
    Logging::write(
      Error,
      "Cassette::populateInputBuffers",
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
      "Cassette::setupInputBuffers",
      "Unable to allocate input buffers."
    );
  }

  if (!populateInputBuffers()) {
    Logging::write(
      Error,
      "Cassette::setupInputBuffers",
      "Unable to populate input buffers."
    );
    return false;
  };

  return true;
}

bool Cassette::freeBuffers() const {
  try {
    free(buffer);

    for (auto i = 0; i < 2; i++) {
      delete inputBuffers[i];
    }
    free(inputBuffers);

    for (int i = 0; i < 2; i++) {
        delete buffersA[i];
        delete buffersB[i];
    }

    free(buffersA);
    free(buffersB);

    for (int i = 0; i < MAX_EFFECTS_CHANNELS; i++) {
      delete effectsChannelsWriteOutBuffer[i][0];
      delete effectsChannelsWriteOutBuffer[i][1];
    }

    free(effectsChannelsWriteOutBuffer);
  } catch (...) {
    Logging::write(
        Error,
        "Cassette::freeBuffers",
        "Unable to free memory for Cassette buffers"
    );
    return false;
  }

  return true;
}

  // TODO:
  // - cleanup when shutdown during playback
int Cassette::play() {
  Logging::write(
    Info,
    "Cassette::play",
    "Playing Cassette..."
  );

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
          "Cassette::play",
          "Read complete"
        );
        break;
    }

    if (audioData.fadeIn > 0.01) {
        audioData.fadeIn -= 0.01;
        audioData.volume += 0.01;
    }

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

  return 0;
};

} // Audio
} // Gj
