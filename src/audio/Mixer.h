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

#include "../AppState.h"
#include "../Logging.h"
#include "./Channel.h"
#include "JackClient.h"
#include "./effects/EffectsChannel.h"
#include "../gui/Mixer/Channel/EffectsChannel/Effects/VstWindow.h"

namespace Gj {
namespace Audio {

class Mixer {
    AppState* gAppState;
    std::shared_ptr<JackClient> jackClient;
    float channelCount;
    std::vector<Effects::EffectsChannel*> effectsChannels;
    std::function<void(sf_count_t, sf_count_t)> updateProgressBarFunc;

    void incorporateLatencySamples(int latencySamples) const;

public:
  explicit Mixer(AppState*);
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

  bool addEffectsChannel();
  bool removeEffectsChannel(int idx);
  [[nodiscard]]
  int effectsOnChannelCount(int idx) const;

  bool addEffectToChannel(int idx, const std::string& effectPath) const;
  void initEditorHostsOnChannel(int idx, std::vector<std::shared_ptr<Gui::VstWindow>>& vstWindows) const;
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

};

} // Audio
} // Gj



#endif //GJAUDIOMIXER_H
