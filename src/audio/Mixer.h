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
#include "AudioCore.h"

namespace Gj {
namespace Audio {

// forward decl
class JackClient;

class Mixer {
  AppState* gAppState;
  std::shared_ptr<JackClient> jackClient;
  float channelCount;
  std::vector<Effects::EffectsChannel*> effectsChannels;
  std::function<void(sf_count_t, sf_count_t)> updateProgressBarFunc;
  std::function<void(jack_ringbuffer_t* eqRingBuffer)> setEqRingBufferFunc;
  std::function<void(jack_ringbuffer_t* vuRingBuffer)> setVuRingBufferFunc;
  std::function<void()> eqAnimationStartFunc;
  std::function<void()> eqAnimationStopFunc;

  void incorporateLatencySamples(int latencySamples) const;

public:
  AudioCore* audioCore;
  Db::Dao* dao;
  explicit Mixer(AppState*, Db::Dao*, AudioCore*);
  ~Mixer();

  [[nodiscard]]
  std::shared_ptr<JackClient> getGjJackClient() const {
    return jackClient;
  };

  Result setPlaybackSpeed(const int newPlaybackSpeed) {
    audioCore->playbackSpeed = static_cast<float>(newPlaybackSpeed) / 100.0f;
    return OK;
  }

  sf_count_t getPlaybackSpeed() const {
    return std::floor(audioCore->playbackSpeed * 100.0f);
  }

  [[nodiscard]]
  std::vector<Effects::EffectsChannel*> getEffectsChannels() const {
    return effectsChannels;
  }

  [[nodiscard]]
  Effects::EffectsChannel* getEffectsChannel(int index) const {
    return effectsChannels.at(index);
  }

  [[nodiscard]]
  int getEffectsChannelsCount() const {
    return static_cast<int>(effectsChannels.size() - 1);
  }

  [[nodiscard]]
  int getTotalChannelsCount() const {
    return static_cast<int>(effectsChannels.size());
  }

  bool addEffectsChannel();
  bool addEffectsChannelFromEntity(const Db::ChannelEntity& channelEntity);
  bool removeEffectsChannel(int idx);
  [[nodiscard]]
  int effectsOnChannelCount(int idx) const;

  bool addEffectToChannel(int channelIndex, const std::string& effectPath) const;
  Result loadEffectOnChannel(const Db::Effect& effectEntity) const;
  void initEditorHostsOnChannel(int idx, std::vector<std::shared_ptr<Gui::VstWindow>>& vstWindows) const;
  void initEditorHostOnChannel(int idx, int newEffectChannel, std::shared_ptr<Gui::VstWindow> vstWindow) const;
  void terminateEditorHostsOnChannel(int idx) const;

  Result setSampleRate(uint32_t sampleRate) const;

  bool replaceEffectOnChannel(int channelIdx, int effectIdx, std::string effectPath) const;
  bool removeEffectFromChannel(int channelIdx, int effectIdx) const;

  int getAudioFramesPerBuffer() const { return gAppState->audioFramesPerBuffer; };
  Result setAudioFramesPerBuffer(jack_nframes_t framesPerBuffer) const;

  bool setGainOnChannel(int channelIdx, float gain) const;

  void setUpdateProgressBarFunc(std::function<void(sf_count_t, sf_count_t)> func) { updateProgressBarFunc = func; };
  std::function<void(sf_count_t, sf_count_t)> getUpdateProgressBarFunc() { return updateProgressBarFunc; };

  void setSetEqRingBufferFunc(std::function<void(jack_ringbuffer_t* eqRingBuffer)> func) { setEqRingBufferFunc = func; };
  std::function<void(jack_ringbuffer_t* eqRingBuffer)> getSetEqRingBufferFunc() { return setEqRingBufferFunc; };

  void setSetVuRingBufferFunc(std::function<void(jack_ringbuffer_t* vuRingBuffer)> func) { setVuRingBufferFunc = func; };
  std::function<void(jack_ringbuffer_t* vuRingBuffer)> getSetVuRingBufferFunc() { return setVuRingBufferFunc; };

  std::string getPluginName(const int channelIdx, const int pluginIndex) const {
    return effectsChannels.at(channelIdx)->getPluginName(pluginIndex);
  };

  Result loadScene();
  int loadSceneByIndex(int sceneIndex);
  int deleteChannels();
  int setChannels(const std::vector<Db::ChannelEntity>& channelEntities);
  int setEffects(const std::vector<Db::Effect>& effects) const;
  int saveScene() const;
  static Result setFrameId(sf_count_t frameId);
};

} // Audio
} // Gj



#endif //GJAUDIOMIXER_H
