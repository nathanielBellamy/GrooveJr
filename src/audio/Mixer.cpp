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

  if (gAppState->getSceneId() == 0) {
    loadSceneByIndex(0);
  } else {
    loadScene();
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

bool Mixer::addEffectsChannelFromEntity(const Db::ChannelEntity& channelEntity) {
    effectsChannels.push_back(
      new Effects::EffectsChannel(
        gAppState,
        jackClient,
        channelEntity
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

bool Mixer::addEffectToChannel(const int channelIndex, const std::string& effectPath) const {
  Logging::write(
    Info,
    "Audio::Mixer::addEffectToChannel",
    "Adding effect " + effectPath + " to channel " + std::to_string(channelIndex)
  );
  if (effectsChannels.at(channelIndex) == nullptr) {
    Logging::write(
      Error,
      "Audio::Mixer::addEffectToChannel",
      "No channel found at idx: " + std::to_string(channelIndex)
    );
    return false;
  }
  return effectsChannels.at(channelIndex)->addReplaceEffect(-1, effectPath);
}

Result Mixer::loadEffectOnChannel(const Db::Effect& effectEntity) const {
  Logging::write(
    Info,
    "Audio::Mixer::loadEffectOnChannel",
    "Adding effect " + effectEntity.filePath + " to channel " + std::to_string(effectEntity.channelIndex)
  );
  if (effectsChannels.at(effectEntity.channelIndex) == nullptr) {
    Logging::write(
      Error,
      "Audio::Mixer::addEffectToChannel",
      "No channel found at idx: " + std::to_string(effectEntity.channelIndex)
    );
    return ERROR;
  }
  return effectsChannels.at(effectEntity.channelIndex)->loadEffect(effectEntity);
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
  Logging::write(
    Info,
    "Audio::Mixer::terminateEditorHostsOnChannel",
    "Terminating editor hosts on channelIndex: " + std::to_string(idx)
  );

  if (idx < effectsChannels.size() && idx >= 0) {
    effectsChannels.at(idx)->terminateEditorHosts();
  } else {
    Logging::write(
      Error,
      "Audio::Mixer::terminateEditorHostsOnChannel",
      "Attempting to terminate editor host on out of range channelIndex: " + std::to_string(idx) + " channelCount: " + std::to_string(effectsChannels.size())
    );
  }

  Logging::write(
    Info,
    "Audio::Mixer::terminateEditorHostsOnChannel",
    "Done terminating editor hosts on channelIndex: " + std::to_string(idx)
  );
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

int Mixer::loadScene() {
  const int sceneId = gAppState->getSceneId();
  Logging::write(
    Info,
    "Audio::Mixer::loadScene",
    "Loading sceneIndex: " + std::to_string(gAppState->getSceneIndex()) + " sceneId: " + std::to_string(sceneId)
  );

  const std::vector<Db::ChannelEntity> channels = dao->sceneRepository.getChannels(sceneId);
  setChannels(channels);
  const std::vector<Db::Effect> effects = dao->sceneRepository.getEffects(sceneId);
  setEffects(effects);

  Logging::write(
    Info,
    "Audio::Mixer::loadSceneById",
    "Loading scene id: " + std::to_string(sceneId)
  );

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

  loadScene();
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
  channelCount = 0;

  Logging::write(
    Info,
    "Audio::Mixer::deleteChannels",
    "Done deleting channels."
  );

  return 0;
}

int Mixer::setChannels(const std::vector<Db::ChannelEntity>& channelEntities) {
  Logging::write(
    Info,
    "Audio::Mixer::setChannels",
    "Setting channels: " + std::to_string(channelEntities.size())
  );

  deleteChannels();

  if (channelEntities.empty()) {
    addEffectsChannel(); // main
    addEffectsChannel(); // channel 1
  } else {
    std::sort(channelEntities.begin(), channelEntities.end());
    for (const auto& channelEntity : channelEntities) {
      addEffectsChannelFromEntity(channelEntity);
    }
  }

  Logging::write(
    Info,
    "Audio::Mixer::setChannels",
    "Done setting channels: " + std::to_string(channelEntities.size())
  );

  return 0;
}

int Mixer::setEffects(const std::vector<Db::Effect> &effects) const {
  Logging::write(
    Info,
    "Audio::Mixer::setEffects",
    "Setting effects."
  );

  std::vector<std::vector<Db::Effect>> effectsByChannel;
  for (const auto& effect : effects) {
    while (effectsByChannel.size() <= effect.channelIndex)
      effectsByChannel.emplace_back();

    effectsByChannel.at(effect.channelIndex).push_back(effect);
  }

  for (const auto& effectsChannelEffects : effectsByChannel) {
    std::sort(effectsChannelEffects.begin(), effectsChannelEffects.end());
    for (const auto& effect : effectsChannelEffects) {
      if (loadEffectOnChannel(effect)) {
        Logging::write(
          Info,
          "Audio::Mixer::setEffects",
          "Loaded effect " + effect.name + " on channel " + std::to_string(effect.channelIndex)
        );
      } else {
        Logging::write(
          Error,
          "Audio::Mixer::setEffects",
          "Could not add effect: " + effect.filePath + " to channel " + std::to_string(effect.channelIndex)
        );
      }
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
    if (!dao->channelRepository.save(effectsChannel->toEntity()))
        Logging::write(
          Error,
          "Audio::Mixer::saveScene",
          "Unable to save channel: " + std::to_string(effectsChannel->getIndex()) + " to sceneId: " + std::to_string(sceneId)
        );

    const int effectCount = effectsChannel->effectCount();
    const auto channelIndex = effectsChannel->getIndex();
    for (int i = 0; i < effectCount; i++) {
      const auto plugin = effectsChannel->getPluginAtIdx(i);
      const auto audioHostComponentStateStream = std::make_unique<ResizableMemoryIBStream>();
      const auto audioHostControllerStateStream = std::make_unique<ResizableMemoryIBStream>();
      const auto editorHostComponentStateStream = std::make_unique<ResizableMemoryIBStream>();
      const auto editorHostControllerStateStream = std::make_unique<ResizableMemoryIBStream>();
      plugin->getState(
        audioHostComponentStateStream.get(),
        audioHostControllerStateStream.get(),
        editorHostComponentStateStream.get(),
        editorHostControllerStateStream.get()
      );
      std::cout << "will get stream sizes" << std::endl;

      const auto audioHostComponentStateSize = getStreamSize(audioHostComponentStateStream.get());
      const auto audioHostControllerStateSize = getStreamSize(audioHostControllerStateStream.get());
      const auto editorHostComponentStateSize = getStreamSize(editorHostComponentStateStream.get());
      const auto editorHostControllerStateSize = getStreamSize(editorHostControllerStateStream.get());

      std::cout << "foooo bar stream sizes"
        << " :: " << audioHostComponentStateSize
        << " :: " << audioHostControllerStateSize
        << " :: " << editorHostControllerStateSize
        << " :: " << editorHostControllerStateSize
        << std::endl;
      std::vector<uint8_t> audioHostComponentBuffer (audioHostComponentStateSize);
      std::vector<uint8_t> audioHostControllerBuffer (audioHostControllerStateSize);
      std::vector<uint8_t> editorHostComponentBuffer (editorHostComponentStateSize);
      std::vector<uint8_t> editorHostControllerBuffer (editorHostControllerStateSize);

      int32 audioHostComponentNumBytesRead = 0;
      int32 audioHostControllerNumBytesRead = 0;
      int32 editorHostComponentNumBytesRead = 0;
      int32 editorHostControllerNumBytesRead = 0;
      std::cout << "foooo bar" << std::endl;

      audioHostComponentStateStream->read(
        audioHostComponentBuffer.data(),
        static_cast<int32>(audioHostComponentStateSize),
        &audioHostComponentNumBytesRead
      );
      std::cout << "fooooo read audiohost component - num bytes: " << audioHostComponentNumBytesRead << std::endl;

      audioHostControllerStateStream->read(
        audioHostControllerBuffer.data(),
        static_cast<int32>(audioHostControllerStateSize),
        &audioHostControllerNumBytesRead
      );
      std::cout << "fooooo read audiohost controller - num bytes: " << audioHostControllerNumBytesRead << std::endl;

      editorHostComponentStateStream->read(
        editorHostComponentBuffer.data(),
        static_cast<int32>(editorHostComponentStateSize),
        &editorHostComponentNumBytesRead
      );
      std::cout << "fooooo read editorHost component - num bytes: " << editorHostComponentNumBytesRead << std::endl;

      editorHostControllerStateStream->read(
        editorHostControllerBuffer.data(),
        static_cast<int32>(editorHostControllerStateSize),
        &editorHostControllerNumBytesRead
      );
      std::cout << "fooooo read editorHost controller - num bytes: " << editorHostControllerNumBytesRead << std::endl;

      std::cout << "fooooo saving effect" << std::endl;
      const auto dbEffect = Db::Effect(
        plugin->path,
        "vst3",
        plugin->name,
        channelIndex,
        i,
        audioHostComponentBuffer,
        audioHostControllerBuffer,
        editorHostComponentBuffer,
        editorHostControllerBuffer
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