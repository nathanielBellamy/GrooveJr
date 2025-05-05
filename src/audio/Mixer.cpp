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
  , channelCount(1.0f)
  {

  Logging::write(
    Info,
    "Audio::Mixer::Mixer()",
    "Instantiating Mixer..."
  );

  jackClient->initialize("GrooveJr");

  if (jackClient->getJackClient() == nullptr) {
    Logging::write(
      Error,
      "Audio::Mixer::Mixer()",
      "Failed to initialize JackClient"
    );
  } else {
    Logging::write(
      Info,
      "Audio::Mixer::Mixer()",
      "Initialized GrooveJr JackClient."
    );
  }

  Logging::write(
    Info,
    "Audio::Mixer::Mixer()",
    "Initialized Mixer."
  );
}

Mixer::~Mixer() {
  Logging::write(
    Info,
    "Audio::Mixer::~Mixer",
    "Destroying Mixer."
  );

  for (const auto channel : effectsChannels) {
    delete channel;
  }
  Logging::write(
    Info,
    "Audio::Mixer::~Mixer",
    "Mixer done delete effectsChannels"
  );

  if (jack_client_close(jackClient->getJackClient())) {
    Logging::write(
      Error,
      "Audio::Mixer::~Mixer",
      "Failed to close JackClient"
    );
  } else {
    Logging::write(
      Info,
      "Audio::Mixer::~Mixer",
      "Closed JackClient"
    );
  }

  Logging::write(
    Info,
    "Audio::Mixer::~Mixer",
    "Destroyed Mixer."
  );
}

bool Mixer::addEffectsChannel() {
    effectsChannels.push_back(
      new Effects::EffectsChannel(
        gAppState,
        jackClient,
        static_cast<int>(effectsChannels.size())
      )
    );
    channelCount++;
    return true;
}

bool Mixer::removeEffectsChannel(const int idx) {
  delete effectsChannels.at(idx);
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
    "Audio::Mixer::addEffectToChannel",
    "Adding effect " + effectPath + " to channel " + std::to_string(idx)
  );
  if (effectsChannels.at(idx) == nullptr) {
    Logging::write(
      Error,
      "Audio::Mixer::addEffectToChannel",
      "No channel found at idx: " + std::to_string(idx)
    );
    return false;
  }
  return effectsChannels.at(idx)->addReplaceEffect(-1, effectPath);
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

bool Mixer::replaceEffectOnChannel(int channelIdx, int effectIdx, std::string effectPath) const {
  return effectsChannels.at(channelIdx)->addReplaceEffect(effectIdx, effectPath);
}

bool Mixer::removeEffectFromChannel(const int channelIdx, const int effectIdx) const {
  return effectsChannels.at(channelIdx)->removeEffect(effectIdx);
}

bool Mixer::setGainOnChannel(int channelIdx, int gain) const {
  std::cout << "Mixer::setGainOnChannel -- c " << channelIdx << "  -- g " << gain << std::endl;
}


} // Audio
} // Gj