//
// Created by ns on 1/12/25.
//

#include "Mixer.h"

namespace Gj {
namespace Audio {

using namespace Steinberg;

Mixer::Mixer(AppState* gAppState, Db::Dao* dao)
  : gAppState(gAppState)
  , dao(dao)
  , jackClient(new JackClient())
  , channelCount(1.0f)
  {

  Logging::write(
    Info,
    "Audio::Mixer::Mixer()",
    "Instantiating Mixer..."
  );

  Logging::write(
    Info,
    "Audio::Mixer::Mixer()",
    "Retrieving effects..."
  );

  loadScene(gAppState->sceneId);

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

int Mixer::effectsOnChannelCount(const int idx) const {
  return effectsChannels.at(idx)->effectCount();
}

void Mixer::initEditorHostsOnChannel(const int idx, std::vector<std::shared_ptr<Gui::VstWindow>>& vstWindows) const {
  return effectsChannels.at(idx)->initEditorHosts(vstWindows);
}

void Mixer::initEditorHostOnChannel(const int idx, const int newEffectChannel, std::shared_ptr<Gui::VstWindow> vstWindow) const {
  return effectsChannels.at(idx)->initEditorHost(newEffectChannel, vstWindow);
}

void Mixer::terminateEditorHostsOnChannel(const int idx) const {
  return effectsChannels.at(idx)->terminateEditorHosts();
}

bool Mixer::replaceEffectOnChannel(const int channelIdx, const int effectIdx, std::string effectPath) const {
  return effectsChannels.at(channelIdx)->addReplaceEffect(effectIdx, effectPath);
}

bool Mixer::removeEffectFromChannel(const int channelIdx, const int effectIdx) const {
  return effectsChannels.at(channelIdx)->removeEffect(effectIdx);
}

bool Mixer::setGainOnChannel(const int channelIdx, const float gain) const {
  return effectsChannels.at(channelIdx)->setGain(gain);
}

int Mixer::loadScene(const int sceneId) {
  Logging::write(
    Info,
    "Audio::Mixer::loadScene",
    "Loading scene " + std::to_string(sceneId)
  );

  const std::vector<Db::Effect> effects = dao->effectRepository.getBySceneId(gAppState->sceneId);
  // TODO: set effects
  for (const auto effect : effects) {
    std::cout << "Mixer load Effects, id = " << effect.id << ", filePath = " << effect.filePath << ", format = " << effect.format << ", name = " << effect.name << ", version = " << effect.version << std::endl;
  }

  return 0;
}

int Mixer::saveScene() const {
  Logging::write(
    Info,
    "Audio::Mixer::saveScene",
    "Saving scene, sceneIndex: " + std::to_string(gAppState->sceneIndex)
  );

  Db::Scene scene = Db::Scene(gAppState->sceneIndex, "Mixer Scene");
  const int sceneId = dao->sceneRepository.save(scene);
  gAppState->sceneId = sceneId;

  for (const auto effectsChannel : effectsChannels) {
    const int effectCount = effectsChannel->effectCount();
    for (int i = 0; i < effectCount; i++) {
      const auto plugin = effectsChannel->getPluginAtIdx(i);
      const auto dbEffect = Db::Effect(
        plugin->path,
        "vst3",
        plugin->name,
        0,
        effectsChannel->getIndex(),
        i,
        0
      );
      dao->effectRepository.save(dbEffect);
    }
  }

  return 0;
}


} // Audio
} // Gj