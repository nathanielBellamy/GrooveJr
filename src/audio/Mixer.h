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

#include "../AppState.h"
#include "../enums/Result.h"
#include "../Logging.h"
#include "../db/Dao.h"
#include "../db/entity/mixer/ChannelEntity.h"
#include "../db/entity/mixer/Plugin.h"
#include "../db/entity/mixer/Scene.h"
// #include "JackClient.h"
#include "./effects/EffectsChannel.h"
#include "../gui/Mixer/Channel/EffectsChannel/Effects/VstWindow.h"
#include "effects/vst3/Util.h"
#include "ThreadStatics.h"

namespace Gj {
namespace Audio {
// forward decl
class JackClient;

class Mixer {
  AppState* gAppState;
  std::shared_ptr<JackClient> jackClient;
  // main channel is effectsChannels.front()
  std::vector<Effects::EffectsChannel*> effectsChannels;
  std::function<void(sf_count_t, sf_count_t)> updateProgressBarFunc;
  std::function<void(jack_ringbuffer_t* eqRingBuffer)> setEqRingBufferFunc;
  std::function<void(jack_ringbuffer_t* vuRingBuffer)> setVuRingBufferFunc;
  std::function<void()> eqAnimationStartFunc;
  std::function<void()> eqAnimationStopFunc;

  void incorporateLatencySamples(int latencySamples) const;

public:
  Db::Dao* dao;

  explicit Mixer(AppState*, Db::Dao*);

  ~Mixer();

  [[nodiscard]]
  std::shared_ptr<JackClient> getGjJackClient() const {
    return jackClient;
  };

  Result setupProcessing() const {
    Result res = OK;

    for (auto& effectsChannel: effectsChannels) {
      if (effectsChannel->setupProcessing() != OK)
        res = ERROR;
    }

    return res;
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
  std::vector<Effects::EffectsChannel*> getEffectsChannels() const {
    return effectsChannels;
  }

  [[nodiscard]]
  Effects::EffectsChannel* getEffectsChannel(const ChannelIndex index) const {
    return effectsChannels.at(index);
  }

  [[nodiscard]]
  ChannelIndex getEffectsChannelsCount() const {
    return effectsChannels.size() - 1;
  }

  [[nodiscard]]
  ChannelIndex getTotalChannelsCount() const {
    return effectsChannels.size();
  }

  Result addEffectsChannel();

  Result addEffectsChannelFromEntity(const Db::ChannelEntity& channelEntity);

  Result removeEffectsChannel(ChannelIndex idx);

  [[nodiscard]]
  PluginIndex pluginsOnChannelCount(ChannelIndex idx) const;

  Result addPluginToChannel(ChannelIndex channelIndex, const PluginPath& pluginPath) const;

  Result loadPluginOnChannel(const Db::Plugin& plugin) const;

  Result initEditorHostsOnChannel(ChannelIndex idx, std::vector<std::shared_ptr<Gui::VstWindow> >& vstWindows) const;

  void initEditorHostOnChannel(ChannelIndex idx, PluginIndex newPluginIndex,
                               std::shared_ptr<Gui::VstWindow> vstWindow) const;

  void terminateEditorHostsOnChannel(ChannelIndex idx) const;

  Result setSampleRate(uint32_t sampleRate) const;

  Result replacePluginOnChannel(ChannelIndex channelIdx, PluginIndex pluginIdx, const PluginPath& pluginPath) const;

  Result removePluginFromChannel(ChannelIndex channelIdx, PluginIndex pluginIdx) const;

  jack_nframes_t getAudioFramesPerBuffer() const { return gAppState->getAudioFramesPerBuffer(); };

  Result setAudioFramesPerBuffer(jack_nframes_t framesPerBuffer) const;

  Result setGainOnChannel(ChannelIndex channelIdx, float gain) const;

  void setUpdateProgressBarFunc(std::function<void(sf_count_t, sf_count_t)> func) { updateProgressBarFunc = func; };

  std::function<void(sf_count_t, sf_count_t)> getUpdateProgressBarFunc() { return updateProgressBarFunc; };

  void setSetEqRingBufferFunc(std::function<void(jack_ringbuffer_t* eqRingBuffer)> func) { setEqRingBufferFunc = func; }
  ;

  std::function<void(jack_ringbuffer_t* eqRingBuffer)> getSetEqRingBufferFunc() { return setEqRingBufferFunc; };

  void setSetVuRingBufferFunc(std::function<void(jack_ringbuffer_t* vuRingBuffer)> func) { setVuRingBufferFunc = func; }
  ;

  std::function<void(jack_ringbuffer_t* vuRingBuffer)> getSetVuRingBufferFunc() { return setVuRingBufferFunc; };

  AtomicStr getPluginName(const ChannelIndex channelIdx, const PluginIndex pluginIndex) const {
    return effectsChannels.at(channelIdx)->getPluginName(pluginIndex);
  };

  Result deleteChannels();

  Result setChannels(std::vector<Db::ChannelEntity> channelEntities);

  Result setPlugins(const std::vector<Db::Plugin>& plugins) const;

  static Result setFrameId(sf_count_t frameId);

  Result loadScene(ID sceneDbId);

  ID newScene() const;

  Result saveScene() const;

  Result saveChannels() const;
};
} // Audio
} // Gj


#endif //GJAUDIOMIXER_H
