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
#include "../db/entity/mixer/Effect.h"
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
  size_t getEffectsChannelsCount() const {
    return effectsChannels.size() - 1;
  }

  [[nodiscard]]
  size_t getTotalChannelsCount() const {
    return effectsChannels.size();
  }

  Result addEffectsChannel();
  Result addEffectsChannelFromEntity(const Db::ChannelEntity& channelEntity);
  Result removeEffectsChannel(ChannelIndex idx);
  [[nodiscard]]
  size_t effectsOnChannelCount(ChannelIndex idx) const;

  bool addEffectToChannel(ChannelIndex channelIndex, const std::string& effectPath) const;
  Result loadEffectOnChannel(const Db::Effect& effectEntity) const;
  void initEditorHostsOnChannel(ChannelIndex idx, std::vector<std::shared_ptr<Gui::VstWindow>>& vstWindows) const;
  void initEditorHostOnChannel(ChannelIndex idx, PluginIndex newEffectIndex, std::shared_ptr<Gui::VstWindow> vstWindow) const;
  void terminateEditorHostsOnChannel(ChannelIndex idx) const;

  Result setSampleRate(uint32_t sampleRate) const;

  bool replaceEffectOnChannel(ChannelIndex channelIdx, PluginIndex effectIdx, const std::string& effectPath) const;
  bool removeEffectFromChannel(ChannelIndex channelIdx, PluginIndex effectIdx) const;

  int getAudioFramesPerBuffer() const { return gAppState->audioFramesPerBuffer; };
  Result setAudioFramesPerBuffer(jack_nframes_t framesPerBuffer) const;

  bool setGainOnChannel(ChannelIndex channelIdx, float gain) const;

  void setUpdateProgressBarFunc(std::function<void(sf_count_t, sf_count_t)> func) { updateProgressBarFunc = func; };
  std::function<void(sf_count_t, sf_count_t)> getUpdateProgressBarFunc() { return updateProgressBarFunc; };

  void setSetEqRingBufferFunc(std::function<void(jack_ringbuffer_t* eqRingBuffer)> func) { setEqRingBufferFunc = func; };
  std::function<void(jack_ringbuffer_t* eqRingBuffer)> getSetEqRingBufferFunc() { return setEqRingBufferFunc; };

  void setSetVuRingBufferFunc(std::function<void(jack_ringbuffer_t* vuRingBuffer)> func) { setVuRingBufferFunc = func; };
  std::function<void(jack_ringbuffer_t* vuRingBuffer)> getSetVuRingBufferFunc() { return setVuRingBufferFunc; };

  AtomicStr getPluginName(const ChannelIndex channelIdx, const PluginIndex pluginIndex) const {
    return effectsChannels.at(channelIdx)->getPluginName(pluginIndex);
  };

  Result deleteChannels();
  Result setChannels(std::vector<Db::ChannelEntity> channelEntities);
  Result setEffects(const std::vector<Db::Effect>& effects) const;
  static Result setFrameId(sf_count_t frameId);
  Result loadScene(ID sceneDbId);
  ID newScene() const;
  Result saveScene() const;
  Result saveChannels() const;
};

} // Audio
} // Gj


#endif //GJAUDIOMIXER_H
