//
// Created by ns on 1/12/25.
//

#include "Mixer.h"

namespace Gj {
namespace Audio {

  using namespace Steinberg;

Mixer::Mixer(AppState* gAppState)
  : gAppState(gAppState)
  , jackClient(new JackClient())
  , mainChannel({ 1.0f, 0.0f })
  , dryChannel({ 1.0f, 0.0f })
  , channelCount(1.0f)
  , inputBuffers(nullptr)
  , buffersA(nullptr)
  , buffersB(nullptr)
  {

  Logging::write(
    Info,
    "Mixer::Mixer()",
    "Instantiating Mixer..."
  );

  jackClient->initialize("GrooveJr");

  if (jackClient->getJackClient() == nullptr) {
    Logging::write(
      Error,
      "Mixer::Mixer()",
      "Failed to initialize JackClient"
    );
  } else {
    Logging::write(
      Info,
      "Mixer::Mixer()",
      "Initialized GrooveJr JackClient."
    );
  }

  allocateBuffersAB();

  Logging::write(
    Info,
    "Mixer::Mixer()",
    "Allocated Buffers."
  );

  Logging::write(
    Info,
    "Mixer::Mixer()",
    "Initialized Mixer."
  );
}

Mixer::~Mixer() {
  Logging::write(
    Info,
    "Mixer::~Mixer",
    "Destroying Mixer."
  );

  for (const auto channel : effectsChannels) {
    delete channel;
  }
  Logging::write(
    Info,
    "Mixer::~Mixer",
    "Mixer done delete effectsChannels"
  );

  if (jack_client_close(jackClient->getJackClient())) {
    Logging::write(
      Error,
      "Mixer::~Mixer",
      "Failed to close JackClient"
    );
  } else {
    Logging::write(
      Error,
      "Mixer::~Mixer",
      "Closed JackClient"
    );
  }

  if (!freeBuffers()) {
    Logging::write(
      Error,
      "Mixer::~Mixer",
      "An error occurred while freeing buffers."
    );
  } else {
    Logging::write(
      Info,
      "Mixer::~Mixer",
      "Done freeing buffers."
    );
  }

  Logging::write(
    Info,
    "Mixer::~Mixer",
    "Destroyed Mixer."
  );
}

bool Mixer::allocateInputBuffers(const sf_count_t frames) {
  if (inputBuffers == nullptr) {
    inputBuffers = static_cast<float**>(
        malloc(2 * frames * sizeof(float))
    );
  } else {
    inputBuffers = static_cast<float**>(
      realloc(inputBuffers, 2 * frames * sizeof(float*))
    );
  }

  if (inputBuffers == nullptr)
    Logging::write(
      Error,
      "Mixer::allocateInputBuffers",
      "Unable to allocate memory for Mixer.inputBuffers"
    );

  for (int c = 0; c < 2; c++) {
    inputBuffers[c] = new float[frames];
  }
}

bool Mixer::populateInputChannels(const sf_count_t frames, float* audioDataBuffer) {
  // de-interlace audio into shared input buffers
  for (int i = 0; i < frames; i++) {
    inputBuffers[0][i] = audioDataBuffer[2 * i];
    inputBuffers[1][i] = audioDataBuffer[2 * i + 1];
  }
}

bool Mixer::setupInputBuffers(sf_count_t frames, float *audioDataBuffer) {
  allocateInputBuffers(frames);
  populateInputChannels(frames, audioDataBuffer);
}


bool Mixer::allocateBuffersAB() {
  buffersA = static_cast<float**>(
  	malloc(2 * gAppState->audioFramesPerBuffer * sizeof(float))
  );
  buffersB = static_cast<float**>(
	malloc(2 * gAppState->audioFramesPerBuffer * sizeof(float))
  );

  if (buffersA == nullptr || buffersB == nullptr) {
	Logging::write(
		Error,
		"EffectsChannel::allocateBuffers",
		"Unable to allocate memory for buffersA or buffersB."
	);
	throw std::runtime_error ("Unable to allocate memory for Mixer buffers.");
  }

  for (int c = 0; c < 2; c++) {
  	buffersA[c] = new float[gAppState->audioFramesPerBuffer];
	buffersB[c] = new float[gAppState->audioFramesPerBuffer];
  }

  return true;
}

bool Mixer::freeBuffers() const {
  try {
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
  } catch (...) {
    Logging::write(
        Error,
        "Mixer::freeBuffers",
        "Unable to free memory for Mixer buffers"
    );
    return false;
  }

  return true;
}

bool Mixer::addEffectsChannel() {
    effectsChannels.push_back(
      new Effects::EffectsChannel(
        gAppState,
        jackClient,
        static_cast<int>(effectsChannels.size()),
        inputBuffers,
        buffersA,
        buffersB
      )
    );
    channelCount++;
    return true;
}

bool Mixer::removeEffectsChannel(const int idx) {
  effectsChannels.erase(effectsChannels.begin() + idx);
  channelCount--;
  return true;
}

bool Mixer::setSampleRate(const int sampleRate) const {
  for (const auto effectsChannel : effectsChannels) {
    effectsChannel->setSampleRate(sampleRate);
  }
  return true;
}

void Mixer::incorporateLatencySamples(const int latencySamples) const {
  if (gAppState->audioFramesPerBuffer > latencySamples) return;

  const double exponent = std::log2(latencySamples);

  gAppState->audioFramesPerBuffer = static_cast<int>(std::pow(2, std::ceil(exponent)));
}

bool Mixer::addEffectToChannel(const int idx, const std::string& effectPath) const {
  Logging::write(
    Info,
    "Mixer::addEffectToChannel",
    "Adding effect " + effectPath + " to channel " + std::to_string(idx)
  );
  if (effectsChannels.at(idx) == nullptr) {
    Logging::write(
      Error,
      "Mixer::addEffectToChannel",
      "No channel found at idx: " + std::to_string(idx)
    );
    return false;
  }
  return effectsChannels.at(idx)->addEffect(effectPath);
}

// called from audio thread
// do not allocate/free memory!
bool Mixer::mixDown(
  jack_default_audio_sample_t* outL,
  jack_default_audio_sample_t* outR,
  const int audioDataIndex,
  const float* audioDataBuffer,
  const int audioDataSfChannels,
  const jack_nframes_t nframes
  ) const {

  // TODO: handle pan/gain

  for (int i = 0; i < nframes; i++) {
    const auto valL = audioDataBuffer[audioDataIndex + 2 * i];
    const auto valR = audioDataBuffer[audioDataIndex + 2 * i + 1];
    // write dry channel output buffer
    outL[i] = (dryChannel.gain * valL) / channelCount;
    outR[i] = (dryChannel.gain * valR ) / channelCount;

    // de-interlace audio into shared input buffers
    inputBuffers[0][i] = valL;
    inputBuffers[1][i] = valR;
  }

  return true;

  for (const auto effectsChannel : effectsChannels) {
    effectsChannel->process();
    if (channelCount == 1.0f) // dry channel only
      break;

    const auto effectsChannelBuffersWriteOut = effectsChannel->getBuffersWriteOut();

    // write processed audio to outputBuffer
    for (int i = 0; i < nframes; i++) {
      outL[i] += ( effectsChannel->channel.gain * effectsChannelBuffersWriteOut[0][i] ) / channelCount;
      outR[i] += ( effectsChannel->channel.gain * effectsChannelBuffersWriteOut[1][i] ) / channelCount;
    }
  }

  return true;
}

int Mixer::effectsOnChannelCount(int idx) const {
  return effectsChannels.at(idx)->effectCount();
}

void Mixer::initEditorHostsOnChannel(int idx, std::vector<std::shared_ptr<Gui::VstWindow>>& vstWindows) const {
  return effectsChannels.at(idx)->initEditorHosts(vstWindows);
}

void Mixer::terminateEditorHostsOnChannel(int idx) const {
  return effectsChannels.at(idx)->terminateEditorHosts();
}


} // Audio
} // Gj