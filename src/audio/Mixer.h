//
// Created by ns on 1/12/25.
//

#ifndef GJAUDIOMIXER_H
#define GJAUDIOMIXER_H

#include <iostream>
#include <cmath>
#include <algorithm>
#include <memory>
#include <sndfile.hh>

#include "public.sdk/source/vst/utility/memoryibstream.h"

#include "../AppState.h"
#include "../enums/Result.h"
#include "../Logging.h"
#include "../db/Dao.h"
#include "../db/entity/ChannelEntity.h"
#include "../db/entity/Effect.h"
#include "../db/entity/Scene.h"
#include "JackClient.h"
#include "./effects/EffectsChannel.h"
#include "../gui/Mixer/Channel/EffectsChannel/Effects/VstWindow.h"

namespace Gj {
namespace Audio {

class Mixer {
    AppState* gAppState;
    Db::Dao* dao;
    std::shared_ptr<JackClient> jackClient;
    float channelCount;
    std::vector<Effects::EffectsChannel*> effectsChannels;
    std::function<void(sf_count_t, sf_count_t)> updateProgressBarFunc;

    void incorporateLatencySamples(int latencySamples) const;

public:
  explicit Mixer(AppState*, Db::Dao*);
  ~Mixer();

  [[nodiscard]]
  jack_client_t* getJackClient() const {
    return jackClient.get()->getJackClient();
  };

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

  bool setSampleRate(int sampleRate) const;

  bool replaceEffectOnChannel(int channelIdx, int effectIdx, std::string effectPath) const;
  bool removeEffectFromChannel(int channelIdx, int effectIdx) const;

  int getAudioFramesPerBuffer() const { return gAppState->audioFramesPerBuffer; };
  void setAudioFramesPerBuffer(int framesPerBuffer) const { gAppState->audioFramesPerBuffer = framesPerBuffer; };

  bool setGainOnChannel(int channelIdx, float gain) const;

  void setUpdateProgressBarFunc(std::function<void(sf_count_t, sf_count_t)> func) { updateProgressBarFunc = func; };
  std::function<void(sf_count_t, sf_count_t)> getUpdateProgressBarFunc() { return updateProgressBarFunc; };

  std::string getPluginName(const int channelIdx, const int pluginIndex) const {
    return effectsChannels.at(channelIdx)->getPluginName(pluginIndex);
  };

  int loadScene();
  int loadSceneByIndex(int sceneIndex);
  int deleteChannels();
  int setChannels(const std::vector<Db::ChannelEntity>& channelEntities);
  int setEffects(const std::vector<Db::Effect>& effects) const;
  int saveScene() const;

  Steinberg::int64 getStreamSize(Steinberg::IBStream* stream) const {
    Steinberg::int64 start = 0;
    Steinberg::int64 size = 0;

    if (stream->seek(0, Steinberg::IBStream::kIBSeekCur, &start) != Steinberg::kResultOk)
      return -1;

    if (stream->seek(0, Steinberg::IBStream::kIBSeekEnd, &size) != Steinberg::kResultOk)
      return -1;

    // Restore the original position
    stream->seek(start, Steinberg::IBStream::kIBSeekSet, nullptr);

    return size;
  }

};

} // Audio
} // Gj



#endif //GJAUDIOMIXER_H
