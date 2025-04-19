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
  , inputBuffersProcessHead(new float*[2])
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

  delete[] inputBuffersProcessHead;

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