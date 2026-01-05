//
// Created by ns on 1/12/25.
//

#ifndef GJAUDIOMIXER_H
#define GJAUDIOMIXER_H

#include <iostream>
#include <cmath>
#include <algorithm>
#include <memory>
#include <atomic>
#include <optional>

#include <sndfile.hh>
#include <jack/ringbuffer.h>

#include "public.sdk/source/vst/utility/memoryibstream.h"

#include "../../state/Core.h"
#include "../../state/mixer/Packet.h"
#include "../../enums/Result.h"
#include "../../Logging.h"
#include "../../db/Dao.h"
#include "../../db/entity/mixer/ChannelEntity.h"
#include "../../db/entity/mixer/Plugin.h"
#include "../../db/entity/mixer/Scene.h"
// #include "JackClient.h"
#include "Channel.h"
#include "../ProcessDataChangeFlag.h"
#include "../../gui/Mixer/Channels/Channel/Plugins/VstWindow.h"
#include "../plugins/vst3/Util.h"
#include "../ThreadStatics.h"

namespace Gj {
namespace Audio {
// forward decl
class JackClient;

namespace Mixer {
class Core {
  State::Core* stateCore;
  std::shared_ptr<JackClient> jackClient;
  // main channel is channels.front()
  std::optional<Channel*> channels[MAX_MIXER_CHANNELS];
  std::function<void(sf_count_t, sf_count_t)> updateProgressBarFunc;
  std::function<void(jack_ringbuffer_t* eqRingBuffer)> setEqRingBufferFunc;
  std::function<void(jack_ringbuffer_t* vuRingBuffer)> setVuRingBufferFunc;
  std::function<void()> eqAnimationStartFunc;
  std::function<void()> eqAnimationStopFunc;
  std::atomic<ProcessDataChangeFlag_t> processDataChangeFlag;

  void incorporateLatencySamples(int latencySamples) const;

public:
  Db::Dao* dao;

  explicit Core(State::Core*, Db::Dao*);

  ~Core();

  ProcessDataChangeFlag_t getProcessDataChangeFlag() const {
    return processDataChangeFlag.load();
  }

  Result setProcessDataChangeFlag(ProcessDataChangeFlag_t flag) {
    processDataChangeFlag.store(flag);
    return OK;
  }

  Result safeDeleteOldPlugins() {
    Logging::write(
      Info,
      "Audio::Mixer::Core::safeDeleteOldPlugins",
      "Safe delete old plugins"
    );
    if (stateCore->isAudioRunning())
      return setProcessDataChangeFlag(ProcessDataChangeFlag::ACKNOWLEDGE);

    return deletePluginsToDelete();
  }


  bool indexHasValidChannel(ChannelIndex idx) {
    if (!channels[idx] || channels[idx].value() == nullptr) {
      channels[idx].reset();
      return false;
    }
    return true;
  }

  template<typename F>
  Result forEachChannel(F&& func) {
    bool warning = false;
    for (ChannelIndex channelIndex = 0; channelIndex < MAX_MIXER_CHANNELS; ++channelIndex) {
      if (!indexHasValidChannel(channelIndex))
        continue;

      try {
        func(channels[channelIndex].value(), channelIndex);
      } catch (...) {
        Logging::write(
          Error,
          "Audio::Mixer::forEachChannel",
          "Error during lambda function for channelIndex: " +
          std::to_string(channelIndex)
        );
        warning = true;
      }
    }
    return warning ? WARNING : OK;
  }

  template<typename F>
  Result runAgainstChannel(ChannelIndex channelIndex, F&& func) {
    if (!indexHasValidChannel(channelIndex)) {
      Logging::write(
        Warning,
        "Audio::Mixer::runOnChannel",
        "Attempting to run lambda on invalid channelIndex: " + std::to_string(channelIndex)
      );
      return WARNING;
    }

    try {
      func(channels[channelIndex].value());
    } catch (...) {
      Logging::write(
        Error,
        "Audio::Mixer::forEachChannel",
        "Error during lambda function for channelIndex: " +
        std::to_string(channelIndex)
      );
      return ERROR;
    }

    return OK;
  }

  [[nodiscard]]
  std::shared_ptr<JackClient> getGjJackClient() const {
    return jackClient;
  };

  Result setupProcessing() {
    Result res = OK;
    const auto setupRes = forEachChannel([this, &res](Channel* channel, const ChannelIndex idx) {
      if (channel->setupProcessing() != OK)
        res = WARNING;
    });

    return res != OK || setupRes != OK ? WARNING : OK;
  }

  Result setPlaybackSpeed(const int newPlaybackSpeed) const {
    auto scene = stateCore->scene.load();
    scene.playbackSpeed = static_cast<float>(newPlaybackSpeed) / 100.0f;
    stateCore->scene.store(scene);
    return OK;
  }

  sf_count_t getPlaybackSpeed() const {
    return std::floor(stateCore->scene.load().playbackSpeed * 100.0f);
  }

  [[nodiscard]]
  std::optional<Channel*>* getChannels() {
    return channels;
  }

  [[nodiscard]]
  std::optional<Channel*> getChannel(const ChannelIndex index) {
    if (index > MAX_MIXER_CHANNELS) {
      Logging::write(
        Warning,
        "Audio::Mixer::getChannel",
        "Attempting to access out of range Channel at index: " + std::to_string(index) + ". MAX_MIXER_CHANNELS: " +
        std::to_string(MAX_MIXER_CHANNELS)
      );
      return std::nullopt;
    }
    return indexHasValidChannel(index)
             ? channels[index]
             : std::nullopt;
  }

  [[nodiscard]]
  ChannelIndex getChannelsCount() {
    int count = 0;
    for (ChannelIndex idx = 1; idx < MAX_MIXER_CHANNELS; ++idx) {
      if (indexHasValidChannel(idx))
        ++count;
    }
    return count;
  }

  [[nodiscard]]
  ChannelIndex getTotalChannelsCount() {
    int count = 0;
    for (ChannelIndex idx = 0; idx < MAX_MIXER_CHANNELS; ++idx) {
      if (indexHasValidChannel(idx))
        ++count;
    }
    return count;
  }

  Result deletePluginsToDelete() {
    Logging::write(
      Info,
      "Audio::Mixer::deletePluginsToDelete",
      "Deleting plugins."
    );
    return forEachChannel([](Channel* channel, const ChannelIndex) {
      channel->deletePluginsToDelete();
    });
  }

  std::optional<ChannelIndex> firstOpenChannelIndex() const;

  Result addChannel();

  Result addChannelFromEntity(const Db::ChannelEntity& channelEntity);

  Result removeChannel(ChannelIndex idx);

  [[nodiscard]]
  PluginIndex getPluginsOnChannelCount(ChannelIndex idx);

  Result addPluginToChannel(ChannelIndex channelIndex, const PluginPath& pluginPath);

  Result loadPluginOnChannel(const Db::Plugin& plugin);

  Result replacePluginOnChannel(ChannelIndex channelIdx, PluginIndex pluginIdx, const PluginPath& pluginPath);

  Result removePluginFromChannel(ChannelIndex channelIdx, PluginIndex pluginIdx);

  Result initEditorHostsOnChannel(ChannelIndex idx, std::vector<std::shared_ptr<Gui::Mixer::VstWindow> >& vstWindows);

  Result initEditorHostOnChannel(ChannelIndex idx, PluginIndex newPluginIndex,
                                 std::shared_ptr<Gui::Mixer::VstWindow> vstWindow);

  Result terminateEditorHostsOnChannel(ChannelIndex idx);

  Result setSampleRate(uint32_t sampleRate);

  jack_nframes_t getAudioFramesPerBuffer() const { return stateCore->getAudioFramesPerBuffer(); };

  Result setAudioFramesPerBuffer(jack_nframes_t framesPerBuffer);

  Result setGainOnChannel(ChannelIndex channelIdx, float gain);

  void setUpdateProgressBarFunc(std::function<void(sf_count_t, sf_count_t)> func) { updateProgressBarFunc = func; };

  std::function<void(sf_count_t, sf_count_t)> getUpdateProgressBarFunc() { return updateProgressBarFunc; };

  void setSetEqRingBufferFunc(std::function<void(jack_ringbuffer_t* eqRingBuffer)> func) { setEqRingBufferFunc = func; }
  ;

  std::function<void(jack_ringbuffer_t* eqRingBuffer)> getSetEqRingBufferFunc() { return setEqRingBufferFunc; };

  void setSetVuRingBufferFunc(std::function<void(jack_ringbuffer_t* vuRingBuffer)> func) { setVuRingBufferFunc = func; }
  ;

  std::function<void(jack_ringbuffer_t* vuRingBuffer)> getSetVuRingBufferFunc() { return setVuRingBufferFunc; };

  AtomicStr getPluginName(const ChannelIndex channelIdx, const PluginIndex pluginIndex) {
    if (!indexHasValidChannel(channelIdx)) {
      Logging::write(
        Warning,
        "Audio::Mixer::getPluginName",
        "Attempting to get name of plugin on channelIndex: " + std::to_string(channelIdx) +
        " but no valid channel found."
      );
      return AtomicStr(" - ");
    }
    return channels[channelIdx].value()->getPluginName(pluginIndex);
  };

  Result deleteChannels();

  Result setChannels(std::vector<Db::ChannelEntity> channelEntities);

  Result setPlugins(const std::vector<Db::Plugin>& plugins);

  static Result setFrameId(sf_count_t frameId);

  Result loadScene(ID sceneDbId);

  ID newScene() const;

  Result saveScene();

  Result saveChannels();

  State::Mixer::Packet toPacket() {
    std::cout << "[Audio::Mixer::toPacket] 0" << std::endl;
    State::Mixer::Packet packet{};

    forEachChannel([&packet](Channel* channel, const ChannelIndex channelIdx) {
      std::cout << "[Audio::Mixer::toPacket] channelIdx " << std::to_string(channelIdx) << std::endl;
      State::Mixer::ChannelPacket channelPacket;
      channelPacket.channelIndex = channelIdx;
      channel->forEachPluginSlot(
        [&channelPacket](const std::optional<Plugins::Vst3::Plugin*> plugin, const PluginIndex pluginIdx) {
          if (!plugin) {
            State::Mixer::PluginSlotPacket emptyPacket;
            emptyPacket.hasValue = false;
            channelPacket.plugins[pluginIdx] = emptyPacket;
          } else {
            channelPacket.plugins[pluginIdx] = {
              true,
              pluginIdx,
              plugin.value()->name,
              plugin.value()->path,
            };
          }
        });
      packet.channels[channelIdx] = channelPacket;
    });

    std::cout << " Mixer::toPacket " << packet.std_str() << std::endl;
    return packet;
  }
};
} // Mixer
} // Audio
} // Gj


#endif //GJAUDIOMIXER_H
