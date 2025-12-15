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

#include "../../AppState.h"
#include "../../enums/Result.h"
#include "../../Logging.h"
#include "../../db/Dao.h"
#include "../../db/entity/mixer/ChannelEntity.h"
#include "../../db/entity/mixer/Plugin.h"
#include "../../db/entity/mixer/Scene.h"
// #include "JackClient.h"
#include "Channel.h"
#include "../../gui/Mixer/Channel/EffectsChannel/Effects/VstWindow.h"
#include "../effects/vst3/Util.h"
#include "../ThreadStatics.h"

namespace Gj {
namespace Audio {
// forward decl
class JackClient;

namespace Mixer {
class Core {
  AppState* gAppState;
  std::shared_ptr<JackClient> jackClient;
  // main channel is effectsChannels.front()
  std::optional<Channel*> effectsChannels[MAX_MIXER_CHANNELS];
  std::function<void(sf_count_t, sf_count_t)> updateProgressBarFunc;
  std::function<void(jack_ringbuffer_t* eqRingBuffer)> setEqRingBufferFunc;
  std::function<void(jack_ringbuffer_t* vuRingBuffer)> setVuRingBufferFunc;
  std::function<void()> eqAnimationStartFunc;
  std::function<void()> eqAnimationStopFunc;

  void incorporateLatencySamples(int latencySamples) const;

public:
  Db::Dao* dao;

  explicit Core(AppState*, Db::Dao*);

  ~Core();


  bool indexHasValidChannel(ChannelIndex idx) {
    if (!effectsChannels[idx] || effectsChannels[idx].value() == nullptr) {
      effectsChannels[idx].reset();
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
        func(effectsChannels[channelIndex].value(), channelIndex);
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
      func(effectsChannels[channelIndex].value());
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
    const auto setupRes = forEachChannel([this, &res](Channel* effectsChannel, const ChannelIndex idx) {
      if (effectsChannel->setupProcessing() != OK)
        res = WARNING;
    });

    return res != OK || setupRes != OK ? WARNING : OK;
  }

  Result setPlaybackSpeed(const int newPlaybackSpeed) const {
    auto scene = gAppState->scene.load();
    scene.playbackSpeed = static_cast<float>(newPlaybackSpeed) / 100.0f;
    gAppState->scene.store(scene);
    return OK;
  }

  sf_count_t getPlaybackSpeed() const {
    return std::floor(gAppState->scene.load().playbackSpeed * 100.0f);
  }

  [[nodiscard]]
  std::optional<Channel*>* getEffectsChannels() {
    return effectsChannels;
  }

  [[nodiscard]]
  std::optional<Channel*> getChannel(const ChannelIndex index) {
    if (index > MAX_MIXER_CHANNELS) {
      Logging::write(
        Warning,
        "Audio::Mixer::getEffectsChannel",
        "Attempting to access out of range Channel at index: " + std::to_string(index) + ". MAX_EFFECTS_CHANNELS: " +
        std::to_string(MAX_MIXER_CHANNELS)
      );
      return std::nullopt;
    }
    return indexHasValidChannel(index)
             ? effectsChannels[index]
             : std::nullopt;
  }

  [[nodiscard]]
  ChannelIndex getEffectsChannelsCount() {
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

  std::optional<ChannelIndex> firstOpenChannelIndex() const;

  Result addEffectsChannel();

  Result addEffectsChannelFromEntity(const Db::ChannelEntity& channelEntity);

  Result removeEffectsChannel(ChannelIndex idx);

  [[nodiscard]]
  PluginIndex getPluginsOnChannelCount(ChannelIndex idx);

  Result addPluginToChannel(ChannelIndex channelIndex, const PluginPath& pluginPath);

  Result loadPluginOnChannel(const Db::Plugin& plugin);

  Result initEditorHostsOnChannel(ChannelIndex idx, std::vector<std::shared_ptr<Gui::VstWindow> >& vstWindows);

  Result initEditorHostOnChannel(ChannelIndex idx, PluginIndex newPluginIndex,
                                 std::shared_ptr<Gui::VstWindow> vstWindow);

  Result terminateEditorHostsOnChannel(ChannelIndex idx);

  Result setSampleRate(uint32_t sampleRate);

  Result replacePluginOnChannel(ChannelIndex channelIdx, PluginIndex pluginIdx, const PluginPath& pluginPath);

  Result removePluginFromChannel(ChannelIndex channelIdx, PluginIndex pluginIdx);

  jack_nframes_t getAudioFramesPerBuffer() const { return gAppState->getAudioFramesPerBuffer(); };

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
    return effectsChannels[channelIdx].value()->getPluginName(pluginIndex);
  };

  Result deleteChannels();

  Result setChannels(std::vector<Db::ChannelEntity> channelEntities);

  Result setPlugins(const std::vector<Db::Plugin>& plugins);

  static Result setFrameId(sf_count_t frameId);

  Result loadScene(ID sceneDbId);

  ID newScene() const;

  Result saveScene();

  Result saveChannels();
};
} // Mixer
} // Audio
} // Gj


#endif //GJAUDIOMIXER_H
