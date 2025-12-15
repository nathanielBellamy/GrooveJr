//
// Created by ns on 1/12/25.
//

#include "Mixer.h"

namespace Gj {
namespace Audio {
using namespace Steinberg;

Mixer::Mixer(AppState* gAppState, Db::Dao* dao)
: gAppState(gAppState)
  , jackClient(new JackClient(this))
  , dao(dao) {
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

  loadScene(gAppState->getSceneDbId());

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

std::optional<PluginIndex> Mixer::firstOpenChannelIndex() const {
  for (ChannelIndex i = 0; i < MAX_EFFECTS_CHANNELS; ++i) {
    if (!effectsChannels[i])
      return std::optional(i);
  }
  return std::nullopt;
}

Result Mixer::addEffectsChannel() {
  const auto firstOpenIndex = firstOpenChannelIndex();
  if (!firstOpenIndex) {
    Logging::write(
      Warning,
      "Audio::Mixer::addEffectsChannel",
      "Attempting to add too many channels."
    );
    return WARNING;
  }

  effectsChannels[firstOpenIndex.value()] =
      new Effects::Channel(
        gAppState,
        jackClient,
        firstOpenIndex.value()
      );
  return OK;
}

Result Mixer::setAudioFramesPerBuffer(const jack_nframes_t framesPerBuffer) {
  bool warning = false;
  gAppState->setAudioFramesPerBuffer(framesPerBuffer);

  const auto setRes = forEachChannel(
    [this, &framesPerBuffer, &warning](Effects::Channel* channel, ChannelIndex index) {
      if (channel->setBlockSize(framesPerBuffer) != OK) {
        Logging::write(
          Warning,
          "Audio::Mixer::setAudioFramesPerBuffer",
          "Failed to set audio frames per buffer on channel " + std::to_string(index)
        );
        warning = true;
      }
    });

  return warning || setRes != OK
           ? WARNING
           : OK;
}


Result Mixer::addEffectsChannelFromEntity(const Db::ChannelEntity& channelEntity) {
  delete effectsChannels[channelEntity.channelIndex].value_or(nullptr);
  effectsChannels[channelEntity.channelIndex] =
      new Effects::Channel(
        gAppState,
        jackClient,
        channelEntity
      );
  return OK;
}

Result Mixer::removeEffectsChannel(const ChannelIndex idx) {
  delete effectsChannels[idx].value_or(nullptr);
  return OK;
}

Result Mixer::setSampleRate(const uint32_t sampleRate) {
  const auto sampleRateD = static_cast<double>(sampleRate);
  return forEachChannel(
    [this, &sampleRateD](Effects::Channel* channel, ChannelIndex) {
      channel->setSampleRate(sampleRateD);
    });
}

void Mixer::incorporateLatencySamples(const int latencySamples) const {
  if (gAppState->audioFramesPerBuffer > latencySamples) return;

  const double exponent = std::log2(latencySamples);

  gAppState->audioFramesPerBuffer = static_cast<int>(std::pow(2, std::ceil(exponent)));
}

Result Mixer::addPluginToChannel(const ChannelIndex channelIndex, const PluginPath& pluginPath) {
  Logging::write(
    Info,
    "Audio::Mixer::addPluginToChannel",
    "Adding plugin " + pluginPath + " to channel " + std::to_string(channelIndex)
  );

  if (!indexHasValidChannel(channelIndex)) {
    Logging::write(
      Error,
      "Audio::Mixer::addPluginToChannel",
      "No channel found at idx: " + std::to_string(channelIndex)
    );
    return ERROR;
  }

  return effectsChannels[channelIndex].value()->addReplacePlugin(std::optional<PluginIndex>(), pluginPath);
}

Result Mixer::loadPluginOnChannel(const Db::Plugin& plugin) {
  Logging::write(
    Info,
    "Audio::Mixer::loadPluginOnChannel",
    "Adding plugin " + plugin.filePath + " to channel " + std::to_string(plugin.channelIndex)
  );
  if (!indexHasValidChannel(plugin.channelIndex)) {
    Logging::write(
      Error,
      "Audio::Mixer::loadPluginOnChannel",
      "No channel found at idx: " + std::to_string(plugin.channelIndex)
    );
    return ERROR;
  }
  return effectsChannels[plugin.channelIndex].value()->loadPlugin(plugin);
}

PluginIndex Mixer::getPluginsOnChannelCount(const ChannelIndex idx) {
  if (!indexHasValidChannel(idx)) {
    Logging::write(
      Warning,
      "Audio::Mixer::pluginsOnChannelCount",
      "Attempting to get plugin count on empty channel " + std::to_string(idx)
    );
    return 0;
  }
  return effectsChannels[idx].value()->pluginCount();
}

Result Mixer::initEditorHostsOnChannel(const ChannelIndex idx,
                                       std::vector<std::shared_ptr<Gui::VstWindow> >& vstWindows) {
  if (!indexHasValidChannel(idx)) {
    Logging::write(
      Warning,
      "Audio::Mixer::initEditorHostsOnChannel",
      "Attempting to init Editorhost on ChannelIndex: " + std::to_string(idx) + " but no channel is not valid."
    );
    return WARNING;
  }
  return effectsChannels[idx].value()->initEditorHosts(vstWindows);
}

Result Mixer::initEditorHostOnChannel(const ChannelIndex idx, const PluginIndex newPluginIndex,
                                      std::shared_ptr<Gui::VstWindow> vstWindow) {
  if (!indexHasValidChannel(idx)) {
    Logging::write(
      Warning,
      "Audio::Mixer::initEditorHostOnChannel",
      "Attempting to init Editorhost on ChannelIndex " + std::to_string(idx) + " but no channel is not valid."
    );
    return WARNING;
  }
  return effectsChannels[idx].value()->initEditorHost(newPluginIndex, vstWindow);
}

Result Mixer::terminateEditorHostsOnChannel(const ChannelIndex idx) {
  Logging::write(
    Info,
    "Audio::Mixer::terminateEditorHostsOnChannel",
    "Terminating editor hosts on channelIndex: " + std::to_string(idx)
  );

  if (!indexHasValidChannel(idx)) {
    Logging::write(
      Error,
      "Audio::Mixer::terminateEditorHostsOnChannel",
      "Attempting to terminate editor host on out of range channelIndex: " + std::to_string(idx) + " channelCount: " +
      std::to_string(getTotalChannelsCount())
    );
    return WARNING;
  }

  return effectsChannels[idx].value()->terminateEditorHosts();
}

Result Mixer::replacePluginOnChannel(const ChannelIndex channelIdx, const PluginIndex pluginIdx,
                                     const PluginPath& pluginPath) {
  if (!indexHasValidChannel(channelIdx)) {
    Logging::write(
      Error,
      "Audio::Mixer::terminateEditorHostsOnChannel",
      "Attempting to replace plugin on channelIndex: " + std::to_string(channelIdx) +
      " but no valid channel found. channelCount: " +
      std::to_string(getTotalChannelsCount())
    );
    return WARNING;
  }
  return effectsChannels[channelIdx].value()->addReplacePlugin(pluginIdx, pluginPath);
}

Result Mixer::removePluginFromChannel(const ChannelIndex channelIdx, const PluginIndex pluginIdx) {
  if (!indexHasValidChannel(channelIdx)) {
    Logging::write(
      Error,
      "Audio::Mixer::terminateEditorHostsOnChannel",
      "Attempting to remove plugin on channelIndex: " + std::to_string(channelIdx) +
      " but no valid channel found. channelCount: " +
      std::to_string(getTotalChannelsCount())
    );
    return WARNING;
  }

  return effectsChannels[channelIdx].value()->removePlugin(pluginIdx);
}

Result Mixer::setGainOnChannel(const ChannelIndex channelIdx, const float gain) {
  if (!indexHasValidChannel(channelIdx)) {
    Logging::write(
      Error,
      "Audio::Mixer::setGainOnChannel",
      "Attempting to set gain on channelIndex: " + std::to_string(channelIdx) +
      " but no valid channel found. channelCount: " +
      std::to_string(getTotalChannelsCount())
    );
    return WARNING;
  }

  return effectsChannels[channelIdx].value()->setGain(gain) ? OK : ERROR;
}

Result Mixer::loadScene(const ID sceneDbId) {
  Logging::write(
    Info,
    "Audio::Mixer::loadScene",
    "Loading sceneDbId: " + std::to_string(sceneDbId)
  );

  const auto sceneOpt = dao->sceneRepository.find(sceneDbId);
  if (!sceneOpt) {
    Logging::write(
      Error,
      "Audio::Mixer::loadScene",
      "Unable to find sceneDbId: " + std::to_string(sceneDbId)
    );
    return ERROR;
  }
  const auto scene = sceneOpt.value();
  Logging::write(
    Info,
    "Audio::Mixer::loadScene",
    "Loading Scene: " + scene.toStdString()
  );
  gAppState->setScene(scene);

  const std::vector<Db::ChannelEntity> channels = dao->sceneRepository.getChannels(scene.id);
  const bool channelsWasEmpty = channels.empty();
  setChannels(channels);

  if (channelsWasEmpty)
    saveChannels();

  const std::vector<Db::Plugin> effects = dao->sceneRepository.getPlugins(scene.id);
  setPlugins(effects);

  Logging::write(
    Info,
    "Audio::Mixer::loadScene",
    "Loaded sceneDbId: " + std::to_string(scene.id)
  );

  return OK;
}

ID Mixer::newScene() const {
  Logging::write(
    Info,
    "Audio::Mixer::newScene",
    "Creating New Scene."
  );

  const auto scene = Db::Scene::base();
  const auto id = dao->sceneRepository.create(scene);

  Logging::write(
    Info,
    "Audio::Mixer::loadSceneById",
    "Created New Scene sceneId: " + std::to_string(id)
  );

  return id;
}

Result Mixer::deleteChannels() {
  Logging::write(
    Info,
    "Audio::Mixer::deleteChannels",
    "Deleting channels."
  );

  const auto delRes = forEachChannel([this](Effects::Channel* channel, const ChannelIndex channelIdx) {
    delete channel;
    effectsChannels[channelIdx].reset();
  });

  if (delRes != OK)
    Logging::write(
      Warning,
      "Audio::Mixer::deleteChannels",
      "A Warning or Error Occurred while deleting channels."
    );

  Logging::write(
    Info,
    "Audio::Mixer::deleteChannels",
    "Done deleting channels."
  );

  return delRes == OK ? OK : WARNING;
}

Result Mixer::setChannels(std::vector<Db::ChannelEntity> channelEntities) {
  Logging::write(
    Info,
    "Audio::Mixer::setChannels",
    "Setting channels. ChannelCount: " + std::to_string(channelEntities.size())
  );

  deleteChannels();

  if (channelEntities.empty())
    for (const auto channel: Db::ChannelEntity::baseChannels())
      channelEntities.push_back(channel);

  std::sort(channelEntities.begin(), channelEntities.end());
  for (const auto& channelEntity: channelEntities)
    addEffectsChannelFromEntity(channelEntity);

  Logging::write(
    Info,
    "Audio::Mixer::setChannels",
    "Done setting channels: " + std::to_string(channelEntities.size())
  );

  return OK;
}

Result Mixer::setPlugins(const std::vector<Db::Plugin>& plugins) {
  Logging::write(
    Info,
    "Audio::Mixer::setPlugins",
    "Setting plugins."
  );

  std::vector<std::vector<Db::Plugin> > pluginsByChannel;
  for (const auto& plugin: plugins) {
    while (pluginsByChannel.size() <= plugin.channelIndex)
      pluginsByChannel.emplace_back();

    pluginsByChannel.at(plugin.channelIndex).push_back(plugin);
  }

  for (const auto& effectsChannelPlugins: pluginsByChannel) {
    std::sort(effectsChannelPlugins.begin(), effectsChannelPlugins.end());
    for (const auto& plugin: effectsChannelPlugins) {
      if (loadPluginOnChannel(plugin) == OK) {
        Logging::write(
          Info,
          "Audio::Mixer::setPlugins",
          "Loaded plugin " + plugin.name + " on channel " + std::to_string(plugin.channelIndex)
        );
      } else {
        Logging::write(
          Error,
          "Audio::Mixer::setPlugins",
          "Could not add plugin: " + plugin.filePath + " to channel " + std::to_string(plugin.channelIndex)
        );
      }
    }
  }

  Logging::write(
    Info,
    "Audio::Mixer::setPlugins",
    "Done setting plugins."
  );
  return OK;
}

Result Mixer::saveScene() {
  Logging::write(
    Info,
    "Audio::Mixer::saveScene",
    "Saving scene."
  );
  const auto scene = dao->sceneRepository.update(gAppState->getScene());
  if (!scene) {
    Logging::write(
      Error,
      "Audio::Mixer::saveScene",
      "Could not update scene."
    );
    return ERROR;
  }

  gAppState->setScene(scene.value());

  if (const int persistRes = dao->appStateRepository.persistAndSet(); persistRes != 0) {
    Logging::write(
      Error,
      "Audio::Mixer::saveScene",
      "Failed to persist state: " + std::to_string(persistRes)
    );
    return ERROR;
  }

  if (const auto saveChannelsRes = saveChannels(); saveChannelsRes != OK) {
    if (saveChannelsRes == ERROR)
      Logging::write(
        Error,
        "Audio::Mixer::saveScene",
        "Failed to save channels."
      );

    return saveChannelsRes;
  };

  return OK;
}

Result Mixer::saveChannels() {
  Result result = OK;
  const auto scene = gAppState->getScene();
  // for (const auto effectsChannel: effectsChannels) {
  const auto saveRes = forEachChannel(
    [this, &scene, &result](Effects::Channel* channel, ChannelIndex channelIndex) {
      if (!dao->channelRepository.save(channel->toEntity())) {
        Logging::write(
          Error,
          "Audio::Mixer::saveScene",
          "Unable to save channel: " + std::to_string(channel->getIndex()) + " to sceneDbId: " +
          std::to_string(scene.id)
        );
        result = ERROR;
      }

      for (PluginIndex i = 0; i < MAX_PLUGINS_PER_CHANNEL; i++) {
        const auto plugin = channel->getPluginAtIdx(i);
        if (!plugin) continue;
        const auto audioHostComponentStateStream = std::make_unique<ResizableMemoryIBStream>(2048);
        const auto audioHostControllerStateStream = std::make_unique<ResizableMemoryIBStream>(2048);
        const auto editorHostComponentStateStream = std::make_unique<ResizableMemoryIBStream>(2048);
        const auto editorHostControllerStateStream = std::make_unique<ResizableMemoryIBStream>(2048);
        plugin.value()->getState(
          audioHostComponentStateStream.get(),
          audioHostControllerStateStream.get(),
          editorHostComponentStateStream.get(),
          editorHostControllerStateStream.get()
        );

        int64 audioHostComponentStateSize = 0;
        if (Effects::Vst3::Util::getStreamSize(audioHostComponentStateStream.get(), &audioHostComponentStateSize) !=
            OK) {
          Logging::write(
            Error,
            "Audio::Mixer::saveScene",
            "Unable to determine stream size for audioHostComponentStateStream"
          );
          result = WARNING;
        }

        int64 audioHostControllerStateSize = 0;
        if (Effects::Vst3::Util::getStreamSize(audioHostControllerStateStream.get(), &audioHostControllerStateSize) !=
            OK) {
          Logging::write(
            Error,
            "Audio::Mixer::saveScene",
            "Unable to determine stream size for audioHostControllerStateStream"
          );
          result = WARNING;
        }

        std::vector<uint8_t> audioHostComponentBuffer(audioHostComponentStateSize);
        std::vector<uint8_t> audioHostControllerBuffer(audioHostControllerStateSize);

        if (audioHostComponentStateSize > 0) {
          int32 audioHostComponentNumBytesRead = 0;
          audioHostComponentStateStream->read(
            audioHostComponentBuffer.data(),
            static_cast<int32>(audioHostComponentStateSize),
            &audioHostComponentNumBytesRead
          );
        }

        if (audioHostControllerStateSize > 0) {
          int32 audioHostControllerNumBytesRead = 0;
          audioHostControllerStateStream->read(
            audioHostControllerBuffer.data(),
            static_cast<int32>(audioHostControllerStateSize),
            &audioHostControllerNumBytesRead
          );
        }

        // TODO: debugged. do we need editorhost state? editorhost seems to get state from audiohost
        const std::vector<uint8_t> editorHostComponentBuffer(0);
        const std::vector<uint8_t> editorHostControllerBuffer(0);
        // if (plugin->editorHost != nullptr) {
        //   if (const auto populateRes = plugin->populateEditorHostStateBuffers(editorHostComponentBuffer, editorHostControllerBuffer);
        //       populateRes != OK) {
        //     Logging::write(
        //       Error,
        //       "Audio::Mixer::saveScene",
        //       "Unable to populate editorHost buffers and thus unable to persist Plugin: " + plugin->name + " Status: " + std::to_string(populateRes)
        //     );
        //   }
        // }

        const auto dbPlugin = Db::Plugin(
          plugin.value()->path,
          "vst3",
          plugin.value()->name,
          channelIndex,
          i,
          audioHostComponentBuffer,
          audioHostControllerBuffer,
          editorHostComponentBuffer,
          editorHostControllerBuffer
        );
        if (!dao->pluginRepository.save(dbPlugin)) {
          Logging::write(
            Error,
            "Audio::Mixer::saveScene",
            "Unable to save plugin: " + dbPlugin.filePath + " to sceneId: " + std::to_string(scene.id)
          );
          result = ERROR;
        }
      }
    });

  if (saveRes != OK) return saveRes;

  return result;
}

Result Mixer::setFrameId(const sf_count_t frameId) {
  Logging::write(
    Info,
    "Audio::Mixer::setFramePercent",
    "Setting frameId: " + std::to_string(frameId)
  );

  ThreadStatics::setFrameId(frameId);
  ThreadStatics::setUserSettingFrameId(true);

  return OK;
}
} // Audio
} // Gj
