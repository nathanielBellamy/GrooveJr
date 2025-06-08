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

  if (gAppState->sceneId == 0) {
    loadSceneByIndex(0);
  } else {
    loadSceneById(gAppState->sceneId);
  }

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

  deleteChannels();

  Logging::write(
    Info,
    "Audio::Mixer::~Mixer",
    "Mixer done deleting effectsChannels"
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

int Mixer::loadSceneById(const int sceneId) {
  Logging::write(
    Info,
    "Audio::Mixer::loadSceneById",
    "Loading scene id: " + std::to_string(sceneId)
  );

  const std::vector<Db::Effect> effects = dao->effectRepository.getBySceneId(gAppState->getSceneId());
  setEffects(effects);

  return 0;
}

int Mixer::loadSceneByIndex(const int sceneIndex) {
  Logging::write(
    Info,
    "Audio::Mixer::loadScene",
    "Loading scene index: " + std::to_string(sceneIndex)
  );

  gAppState->setSceneIndex(sceneIndex);
  const int sceneId = dao->sceneRepository.findOrCreateBySceneIndex(sceneIndex);
  Logging::write(
    Info,
    "Audio::Mixer::loadSceneByIndex",
    "Found sceneIndex: " + std::to_string(sceneIndex) + " with sceneId: " + std::to_string(sceneId)
  );

  gAppState->setSceneId(sceneId);
  if (const int persistRes = dao->appStateRepository.persistAndSet(); persistRes != 0) {
    Logging::write(
      Error,
      "Audio::Mixer::loadSceneByIndex",
      "Failed to persist state: " + std::to_string(persistRes)
    );
    return persistRes;
  }

  loadSceneById(sceneId);
  return 0;
}

int Mixer::deleteChannels() {
  Logging::write(
    Info,
    "Audio::Mixer::deleteChannels",
    "Deleting channels."
  );

  for (const auto effectsChannel : effectsChannels)
    delete effectsChannel;

  effectsChannels.clear();

  Logging::write(
    Info,
    "Audio::Mixer::deleteChannels",
    "Done deleting channels."
  );

  return 0;
}


int Mixer::setEffects(const std::vector<Db::Effect> &effects) {
  deleteChannels();

  Logging::write(
    Info,
    "Audio::Mixer::setEffects",
    "Setting effects."
  );

  std::vector<std::vector<Db::Effect>> effectsByChannel;
  for (const auto& effect : effects) {
    while (effectsByChannel.size() <= effect.channelIndex) {
      effectsByChannel.emplace_back();
    }

    effectsByChannel.at(effect.channelIndex).push_back(effect);
  }

  while (effectsChannels.size() < effectsByChannel.size() || effectsChannels.size() < 2) {
    addEffectsChannel();
  }

  for (const auto& effectsChannelEffects : effectsByChannel) {
    std::sort(effectsChannelEffects.begin(), effectsChannelEffects.end());
    for (const auto& effect : effectsChannelEffects) {
      if (!addEffectToChannel(effect.channelIndex, effect.filePath))
        Logging::write(
          Error,
          "Audio::Mixer::setEffects",
          "Could not add effect: " + effect.filePath + " to channel " + std::to_string(effect.channelIndex)
        );
    }
  }

  Logging::write(
    Info,
    "Audio::Mixer::setEffects",
    "Done setting effects."
  );
  return 0;
}

int Mixer::saveScene() const {
  Logging::write(
    Info,
    "Audio::Mixer::saveScene",
    "Saving scene, sceneIndex: " + std::to_string(gAppState->sceneIndex)
  );

  const auto scene = Db::Scene(gAppState->sceneIndex, "Mixer Scene");
  const int sceneId = dao->sceneRepository.save(scene);
  gAppState->setSceneId(sceneId);
  if (const int persistRes = dao->appStateRepository.persistAndSet(); persistRes != 0) {
    Logging::write(
      Error,
      "Audio::Mixer::saveScene",
      "Failed to persist state: " + std::to_string(persistRes)
    );
    return persistRes;
  }

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
      if (!dao->effectRepository.save(dbEffect))
        Logging::write(
          Error,
          "Audio::Mixer::saveScene",
          "Unable to save effect: " + dbEffect.filePath + " to sceneId: " + std::to_string(sceneId)
        );
    }
  }

  return sceneId;
}

} // Audio
} // Gj