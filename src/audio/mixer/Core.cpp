//
// Created by ns on 1/12/25.
//

#include "Core.h"

namespace Gj {
namespace Audio {
namespace Mixer {
using namespace Steinberg;

Core::Core(State::Core* stateCore, Db::Dao* dao)
: stateCore(stateCore)
  , jackClient(new JackClient(this))
  , dao(dao) {
  Logging::write(
    Info,
    "Audio::Mixer::Core()",
    "Instantiating Mixer..."
  );

  loadScene(stateCore->getSceneDbId());

  jackClient->initialize("GrooveJr");

  if (jackClient->getJackClient() == nullptr) {
    Logging::write(
      Error,
      "Audio::Mixer::Core()",
      "Failed to initialize JackClient"
    );
  } else {
    Logging::write(
      Info,
      "Audio::Mixer::Core()",
      "Initialized GrooveJr JackClient."
    );
  }

  Logging::write(
    Info,
    "Audio::Mixer::Core()",
    "Initialized Mixer."
  );
}

Core::~Core() {
  Logging::write(
    Info,
    "Audio::Mixer::~Core",
    "Destroying Mixer."
  );

  deleteChannels();

  Logging::write(
    Info,
    "Audio::Mixer::~Core",
    "Mixer done deleting channels"
  );

  if (jack_client_close(jackClient->getJackClient())) {
    Logging::write(
      Error,
      "Audio::Mixer::~Core",
      "Failed to close JackClient"
    );
  } else {
    Logging::write(
      Info,
      "Audio::Mixer::~Core",
      "Closed JackClient"
    );
  }

  Logging::write(
    Info,
    "Audio::Mixer::~Core",
    "Destroyed Mixer."
  );
}

std::optional<PluginIndex> Core::firstOpenChannelIndex() const {
  for (ChannelIndex i = 0; i < MAX_MIXER_CHANNELS; ++i) {
    if (!channels[i])
      return std::optional(i);
  }
  return std::nullopt;
}

Result Core::addChannel() {
  const auto firstOpenIndex = firstOpenChannelIndex();
  if (!firstOpenIndex) {
    Logging::write(
      Warning,
      "Audio::Mixer::Core::addChannel",
      "Attempting to add too many channels."
    );
    return WARNING;
  }

  channels[firstOpenIndex.value()] =
      new Channel(
        stateCore,
        jackClient,
        firstOpenIndex.value()
      );
  return OK;
}

Result Core::setAudioFramesPerBuffer(const jack_nframes_t framesPerBuffer) {
  bool warning = false;
  stateCore->setAudioFramesPerBuffer(framesPerBuffer);

  const auto setRes = forEachChannel(
    [this, &framesPerBuffer, &warning](Channel* channel, ChannelIndex index) {
      if (channel->setBlockSize(framesPerBuffer) != OK) {
        Logging::write(
          Warning,
          "Audio::Mixer::Core::setAudioFramesPerBuffer",
          "Failed to set audio frames per buffer on channel " + std::to_string(index)
        );
        warning = true;
      }
    });

  return warning || setRes != OK
           ? WARNING
           : OK;
}


Result Core::addChannelFromEntity(const Db::ChannelEntity& channelEntity) {
  delete channels[channelEntity.channelIndex].value_or(nullptr);
  channels[channelEntity.channelIndex] =
      new Channel(
        stateCore,
        jackClient,
        channelEntity
      );
  return OK;
}

Result Core::removeChannel(const ChannelIndex idx) {
  delete channels[idx].value_or(nullptr);
  return OK;
}

Result Core::setSampleRate(const uint32_t sampleRate) {
  const auto sampleRateD = static_cast<double>(sampleRate);
  return forEachChannel(
    [this, &sampleRateD](Channel* channel, ChannelIndex) {
      channel->setSampleRate(sampleRateD);
    });
}

void Core::incorporateLatencySamples(const int latencySamples) const {
  if (stateCore->audioFramesPerBuffer > latencySamples) return;

  const double exponent = std::log2(latencySamples);

  stateCore->audioFramesPerBuffer = static_cast<int>(std::pow(2, std::ceil(exponent)));
}

Result Core::addPluginToChannel(const ChannelIndex channelIndex, const PluginPath& pluginPath) {
  Logging::write(
    Info,
    "Audio::Mixer::Core::addPluginToChannel",
    "Adding plugin " + pluginPath + " to channel " + std::to_string(channelIndex)
  );

  if (!indexHasValidChannel(channelIndex)) {
    Logging::write(
      Error,
      "Audio::Mixer::Core::addPluginToChannel",
      "No channel found at idx: " + std::to_string(channelIndex)
    );
    return ERROR;
  }

  const auto res = channels[channelIndex].value()->addReplacePlugin(std::optional<PluginIndex>(), pluginPath);
  safeDeleteOldPlugins();
  return res;
}

Result Core::loadPluginOnChannel(const Db::Plugin& plugin) {
  Logging::write(
    Info,
    "Audio::Mixer::Core::loadPluginOnChannel",
    "Adding plugin " + plugin.filePath + " to channel " + std::to_string(plugin.channelIndex)
  );
  if (!indexHasValidChannel(plugin.channelIndex)) {
    Logging::write(
      Error,
      "Audio::Mixer::Core::loadPluginOnChannel",
      "No channel found at idx: " + std::to_string(plugin.channelIndex)
    );
    return ERROR;
  }

  const auto res = channels[plugin.channelIndex].value()->loadPlugin(plugin);
  safeDeleteOldPlugins();
  return res;
}

Result Core::replacePluginOnChannel(const ChannelIndex channelIdx, const PluginIndex pluginIdx,
                                    const PluginPath& pluginPath) {
  if (!indexHasValidChannel(channelIdx)) {
    Logging::write(
      Error,
      "Audio::Mixer::Core::terminateEditorHostsOnChannel",
      "Attempting to replace plugin on channelIndex: " + std::to_string(channelIdx) +
      " but no valid channel found. channelCount: " +
      std::to_string(getTotalChannelsCount())
    );
    return WARNING;
  }

  const auto res = channels[channelIdx].value()->addReplacePlugin(pluginIdx, pluginPath);
  safeDeleteOldPlugins();
  return res;
}

Result Core::removePluginFromChannel(const ChannelIndex channelIdx, const PluginIndex pluginIdx) {
  if (!indexHasValidChannel(channelIdx)) {
    Logging::write(
      Error,
      "Audio::Mixer::Core::terminateEditorHostsOnChannel",
      "Attempting to remove plugin on channelIndex: " + std::to_string(channelIdx) +
      " but no valid channel found. channelCount: " +
      std::to_string(getTotalChannelsCount())
    );
    return WARNING;
  }

  const auto res = channels[channelIdx].value()->removePlugin(pluginIdx);
  safeDeleteOldPlugins();
  return res;
}

PluginIndex Core::getPluginsOnChannelCount(const ChannelIndex idx) {
  if (!indexHasValidChannel(idx)) {
    Logging::write(
      Warning,
      "Audio::Mixer::Core::pluginsOnChannelCount",
      "Attempting to get plugin count on empty channel " + std::to_string(idx)
    );
    return 0;
  }
  return channels[idx].value()->pluginCount();
}

Result Core::initEditorHostsOnChannel(const ChannelIndex idx,
                                      const std::shared_ptr<Gui::Mixer::VstWindow>* vstWindows) {
  if (!indexHasValidChannel(idx)) {
    Logging::write(
      Warning,
      "Audio::Mixer::Core::initEditorHostsOnChannel",
      "Attempting to init Editorhost on ChannelIndex: " + std::to_string(idx) + " but no channel is not valid."
    );
    return WARNING;
  }

  const auto res = channels[idx].value()->initEditorHosts(vstWindows);
  Logging::write(
    res == OK ? Info : Warning,
    "Audio::Mixer::Core::initEditorHostsOnChannel",
    "Initialized editor hosts on channel " + std::to_string(idx) + "."
  );
  return res;
}

Result Core::initEditorHostOnChannel(const ChannelIndex idx, const PluginIndex newPluginIndex,
                                     std::shared_ptr<Gui::Mixer::VstWindow> vstWindow) {
  if (!indexHasValidChannel(idx)) {
    Logging::write(
      Warning,
      "Audio::Mixer::Core::initEditorHostOnChannel",
      "Attempting to init Editorhost on ChannelIndex " + std::to_string(idx) + " but no channel is not valid."
    );
    return WARNING;
  }
  return channels[idx].value()->initEditorHost(newPluginIndex, vstWindow);
}

Result Core::terminateEditorHostsOnChannel(const ChannelIndex idx) {
  Logging::write(
    Info,
    "Audio::Mixer::Core::terminateEditorHostsOnChannel",
    "Terminating editor hosts on channelIndex: " + std::to_string(idx)
  );

  if (!indexHasValidChannel(idx)) {
    Logging::write(
      Error,
      "Audio::Mixer::Core::terminateEditorHostsOnChannel",
      "Attempting to terminate editor host on out of range channelIndex: " + std::to_string(idx) + " channelCount: " +
      std::to_string(getTotalChannelsCount())
    );
    return WARNING;
  }

  return channels[idx].value()->terminateEditorHosts();
}

Result Core::setGainOnChannel(const ChannelIndex channelIdx, const float gain) {
  if (!indexHasValidChannel(channelIdx)) {
    Logging::write(
      Error,
      "Audio::Mixer::Core::setGainOnChannel",
      "Attempting to set gain on channelIndex: " + std::to_string(channelIdx) +
      " but no valid channel found. channelCount: " +
      std::to_string(getTotalChannelsCount())
    );
    return WARNING;
  }

  return channels[channelIdx].value()->setGain(gain) ? OK : ERROR;
}

Result Core::loadScene(const ID sceneDbId) {
  Logging::write(
    Info,
    "Audio::Mixer::Core::loadScene",
    "Loading sceneDbId: " + std::to_string(sceneDbId)
  );

  const auto sceneOpt = dao->sceneRepository.find(sceneDbId);
  if (!sceneOpt) {
    Logging::write(
      Error,
      "Audio::Mixer::Core::loadScene",
      "Unable to find sceneDbId: " + std::to_string(sceneDbId)
    );
    return ERROR;
  }
  const auto scene = sceneOpt.value();
  Logging::write(
    Info,
    "Audio::Mixer::Core::loadScene",
    "Loading Scene: " + scene.toStdString()
  );
  stateCore->setScene(scene);

  const std::vector<Db::ChannelEntity> channels = dao->sceneRepository.getChannels(scene.id);
  const bool channelsWasEmpty = channels.empty();
  setChannels(channels);

  if (channelsWasEmpty)
    saveChannels();

  const std::vector<Db::Plugin> plugins = dao->sceneRepository.getPlugins(scene.id);
  setPlugins(plugins);

  Logging::write(
    Info,
    "Audio::Mixer::Core::loadScene",
    "Loaded sceneDbId: " + std::to_string(scene.id)
  );

  return OK;
}

ID Core::newScene() const {
  Logging::write(
    Info,
    "Audio::Mixer::Core::newScene",
    "Creating New Scene."
  );

  const auto scene = Db::Scene::base();
  const auto id = dao->sceneRepository.create(scene);

  Logging::write(
    Info,
    "Audio::Mixer::Core::loadSceneById",
    "Created New Scene sceneId: " + std::to_string(id)
  );

  return id;
}

Result Core::deleteChannels() {
  Logging::write(
    Info,
    "Audio::Mixer::Core::deleteChannels",
    "Deleting channels."
  );

  const auto delRes = forEachChannel([this](Channel* channel, const ChannelIndex channelIdx) {
    delete channel;
    channels[channelIdx].reset();
  });

  if (delRes != OK)
    Logging::write(
      Warning,
      "Audio::Mixer::Core::deleteChannels",
      "A Warning or Error Occurred while deleting channels."
    );

  Logging::write(
    Info,
    "Audio::Mixer::Core::deleteChannels",
    "Done deleting channels."
  );

  return delRes == OK ? OK : WARNING;
}

Result Core::setChannels(std::vector<Db::ChannelEntity> channelEntities) {
  Logging::write(
    Info,
    "Audio::Mixer::Core::setChannels",
    "Setting channels. ChannelCount: " + std::to_string(channelEntities.size())
  );

  deleteChannels();

  if (channelEntities.empty())
    for (const auto channel: Db::ChannelEntity::baseChannels())
      channelEntities.push_back(channel);

  std::sort(channelEntities.begin(), channelEntities.end());
  for (const auto& channelEntity: channelEntities)
    addChannelFromEntity(channelEntity);

  Logging::write(
    Info,
    "Audio::Mixer::Core::setChannels",
    "Done setting channels: " + std::to_string(channelEntities.size())
  );

  return OK;
}

Result Core::setPlugins(const std::vector<Db::Plugin>& plugins) {
  Logging::write(
    Info,
    "Audio::Mixer::Core::setPlugins",
    "Setting plugins."
  );

  std::vector<std::vector<Db::Plugin> > pluginsByChannel;
  for (const auto& plugin: plugins) {
    while (pluginsByChannel.size() <= plugin.channelIndex)
      pluginsByChannel.emplace_back();

    pluginsByChannel.at(plugin.channelIndex).push_back(plugin);
  }

  for (const auto& plugins: pluginsByChannel) {
    std::sort(plugins.begin(), plugins.end());
    for (const auto& plugin: plugins) {
      if (loadPluginOnChannel(plugin) == OK) {
        Logging::write(
          Info,
          "Audio::Mixer::Core::setPlugins",
          "Loaded plugin " + plugin.name + " on channel " + std::to_string(plugin.channelIndex)
        );
      } else {
        Logging::write(
          Error,
          "Audio::Mixer::Core::setPlugins",
          "Could not add plugin: " + plugin.filePath + " to channel " + std::to_string(plugin.channelIndex)
        );
      }
    }
  }

  Logging::write(
    Info,
    "Audio::Mixer::Core::setPlugins",
    "Done setting plugins."
  );
  return OK;
}

Result Core::saveScene() {
  Logging::write(
    Info,
    "Audio::Mixer::Core::saveScene",
    "Saving scene."
  );
  const auto scene = dao->sceneRepository.update(stateCore->getScene());
  if (!scene) {
    Logging::write(
      Error,
      "Audio::Mixer::Core::saveScene",
      "Could not update scene."
    );
    return ERROR;
  }

  stateCore->setScene(scene.value());

  if (const int persistRes = dao->appStateRepository.persistAndSet(); persistRes != 0) {
    Logging::write(
      Error,
      "Audio::Mixer::Core::saveScene",
      "Failed to persist state: " + std::to_string(persistRes)
    );
    return ERROR;
  }

  if (const auto saveChannelsRes = saveChannels(); saveChannelsRes != OK) {
    if (saveChannelsRes == ERROR)
      Logging::write(
        Error,
        "Audio::Mixer::Core::saveScene",
        "Failed to save channels."
      );

    return saveChannelsRes;
  };

  return OK;
}

Result Core::saveChannels() {
  Result result = OK;
  const auto scene = stateCore->getScene();
  const auto saveRes = forEachChannel(
    [this, &scene, &result](Channel* channel, ChannelIndex channelIndex) {
      if (!dao->channelRepository.save(channel->toEntity())) {
        Logging::write(
          Error,
          "Audio::Mixer::Core::saveScene",
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
        if (Plugins::Vst3::Util::getStreamSize(audioHostComponentStateStream.get(), &audioHostComponentStateSize) !=
            OK) {
          Logging::write(
            Error,
            "Audio::Mixer::Core::saveScene",
            "Unable to determine stream size for audioHostComponentStateStream"
          );
          result = WARNING;
        }

        int64 audioHostControllerStateSize = 0;
        if (Plugins::Vst3::Util::getStreamSize(audioHostControllerStateStream.get(), &audioHostControllerStateSize) !=
            OK) {
          Logging::write(
            Error,
            "Audio::Mixer::Core::saveScene",
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
        //       "Audio::Mixer::Core::saveScene",
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
            "Audio::Mixer::Core::saveScene",
            "Unable to save plugin: " + dbPlugin.filePath + " to sceneId: " + std::to_string(scene.id)
          );
          result = ERROR;
        }
      }
    });

  if (saveRes != OK) return saveRes;

  return result;
}

Result Core::setFrameId(const sf_count_t frameId) {
  Logging::write(
    Info,
    "Audio::Mixer::Core::setFramePercent",
    "Setting frameId: " + std::to_string(frameId)
  );

  ThreadStatics::setFrameId(frameId);
  ThreadStatics::setUserSettingFrameId(true);

  return OK;
}
} // Mixer
} // Audio
} // Gj
