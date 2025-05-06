//
// Created by ns on 1/12/25.
//

#ifndef EFFECTSCHANNEL_H
#define EFFECTSCHANNEL_H

#include <memory>

#include "./vst3/Plugin.h"
#include "../ChannelAtomic.h"
#include "../../AppState.h"
#include "../Logging.h"
#include "../JackClient.h"

namespace Gj {
namespace Audio {
namespace Effects {

class EffectsChannel {
  // NOTE:
  // fx0:inputBuffers->buffersA, fx1:buffersA->buffersB, fx2:buffersB->buffersA, fx3:buffersA->buffersB, ...
  AppState* gAppState;
  std::shared_ptr<JackClient> jackClient;
  int index;
  std::vector<Vst3::Plugin*> vst3Plugins;

  public:
    ChannelAtomic channel;

    EffectsChannel(
      AppState* gAppState,
      std::shared_ptr<JackClient> jackClient,
      int index
    );
    ~EffectsChannel();

    [[nodiscard]]
    int getIndex() const { return index; }

    [[nodiscard]]
    Vst3::Plugin* getPluginAtIdx(const int idx) const {
      return vst3Plugins.at(idx);
    };

    float getGain() const {
      return channel.gain.load();
    };

    bool setGain(float gain) {
      channel.gain.store(gain);
      return true;
    };

    float getPan() const {
      return channel.pan.load();
    };

    bool setPan(float pan) {
      channel.pan.store(pan);
      return true;
    };

    bool addReplaceEffect(int effectIdx, const std::string& effectPath);
    void setSampleRate(int sampleRate) const;

    int effectCount() const;
    void initEditorHosts(const std::vector<std::shared_ptr<Gui::VstWindow>>& vstWindows) const;
    void terminateEditorHosts() const;

    bool removeEffect(int effectIdx);

};

} // Effects
} // Audio
} // Gj



#endif //EFFECTCHANNEL_H
