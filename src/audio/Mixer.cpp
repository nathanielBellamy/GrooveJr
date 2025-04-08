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
  , effectsChannels()
  , inputBuffers(nullptr)
  {

  Logging::write(
    Info,
    "Mixer::Mixer()",
    "Instantiating Mixer..."
  );

  jackClient->initialize("GrooveJr-JACK");

  Logging::write(
    Info,
    "Mixer::Mixer()",
    "Initialized GrooveJr JackClient."
  );

  allocateInputBuffers();

  outputBuffer = new float[gAppState->audioFramesPerBuffer * 2];

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
  for (const auto channel : effectsChannels) {
    delete channel;
  }

  Logging::write(
    Info,
    "Mixer::~Mixer",
    "Mixer done delete effectsChannels"
  );

  freeInputBuffers();

  delete outputBuffer;
}

bool Mixer::allocateInputBuffers() {
  // TODO: don't assume stereo
  inputBuffers = static_cast<float**>(
      malloc(2 * gAppState->audioFramesPerBuffer * sizeof(float))
  );

  if (inputBuffers == nullptr)
    Logging::write(
      Error,
      "Mixer::allocateInputBuffers",
      "Unable to allocate memory for Mixer.inputBuffers"
    );

  for (int c = 0; c < 2; c++) {
    inputBuffers[c] = new float[gAppState->audioFramesPerBuffer];
  }

  return true;
}

bool Mixer::freeInputBuffers() const {
  for (auto i = 0; i < 2; i++) {
    delete inputBuffers[i];
  }
  free(inputBuffers);

  return true;
}

bool Mixer::addEffectsChannel() {
    effectsChannels.push_back(
      new Effects::EffectsChannel(
        gAppState,
        static_cast<int>(effectsChannels.size()),
        inputBuffers
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

bool Mixer::mixDown(
  const int audioDataIndex,
  const float* audioDataBuffer,
  const int audioDataSfChannels,
  const int framesPerBuffer
  ) const {
  // called from audio thread
  // do not allocate/free memory!

  // TODO: handle pan/gain

  for (int c = 0; c < audioDataSfChannels; c++) { // TODO: at the moment, we can assume audioDataSfChannels = 2
    for (int i = 0; i < framesPerBuffer; i++) {
      const auto val = audioDataBuffer[audioDataIndex + 2 * i + c];
      // write dry channel output buffer
      outputBuffer[2 * i + c] = (dryChannel.gain * val) / channelCount;

      // de-interlace audio into shared effects input buffers
      inputBuffers[c][i] = val;
    }
  }

  for (const auto effectsChannel : effectsChannels) {
    effectsChannel->process();
    if (channelCount == 1.0f) // dry channel only
      break;

    const auto effectsChannelBuffersWriteOut = effectsChannel->getBuffersWriteOut();

    // write processed audio to outputBuffer
    for (int c = 0; c < audioDataSfChannels; c++) {
      for (int i = 0; i < framesPerBuffer; i++) {
        outputBuffer[2 * i + c] +=
          ( effectsChannel->channel.gain * effectsChannelBuffersWriteOut[c][i] ) / channelCount;
      }
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