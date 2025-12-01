//
// Created by ns on 1/12/25.
//

#ifndef EFFECTSCHANNEL_H
#define EFFECTSCHANNEL_H

#include <memory>
#include <optional>
#include <string>

#include "./vst3/Plugin.h"
#include "../Channel.h"
#include "../../AppState.h"
#include "../../enums/Result.h"
#include "../../db/entity/mixer/Effect.h"
#include "../Logging.h"
#include "../JackClient.h"
#include "../../types/AtomicStr.h"
#include "../../types/Types.h"

namespace Gj {
namespace Audio {
namespace Effects {

class EffectsChannel {
  // NOTE:
  // fx0:inputBuffers->buffersA, fx1:buffersA->buffersB, fx2:buffersB->buffersA, fx3:buffersA->buffersB, ...
  AppState* gAppState;
  std::shared_ptr<JackClient> jackClient;
  ID id { 0 };
  ChannelIndex index;
  AtomicStr name { "Channel" };
  std::vector<Vst3::Plugin*> vst3Plugins;

  public:
    Channel channel;

    EffectsChannel(
      AppState* gAppState,
      std::shared_ptr<JackClient> jackClient,
      ChannelIndex index
    );
    EffectsChannel(
      AppState* gAppState,
      std::shared_ptr<JackClient> jackClient,
      const Db::ChannelEntity& channelEntity
    );
    ~EffectsChannel();

    [[nodiscard]]
    ChannelIndex getIndex() const { return index; }

    [[nodiscard]]
    Vst3::Plugin* getPluginAtIdx(const ChannelIndex idx) const {
      return vst3Plugins.at(idx);
    };

    float getGain() const {
      return channel.gain.load();
    };

    bool setGain(const float gain) {
      channel.gain.store(gain);
      return true;
    };

    float getGainL() const {
      return channel.gainL.load();
    };

    bool setGainL(float gain) {
      channel.gainL.store(gain);
      return true;
    };

    float getGainR() const {
      return channel.gainR.load();
    };

    bool setGainR(float gain) {
      channel.gainR.store(gain);
      return true;
    };

    float getMute() const {
      return channel.mute.load();
    }

    float getMuteL() const {
      return channel.muteL.load();
    }

    float getMuteR() const {
      return channel.muteR.load();
    }

    float toggleMute() {
      const float mute = channel.mute.load();
      if (mute == 1.0f) {
        channel.mute.store(0.0f);
        return 0.0f;
      }
      if (mute == 0.0f) {
        channel.mute.store(1.0f);
        return 1.0f;
      }
      return 0.0f;
    }

    float toggleMuteL() {
      const float mute = channel.muteL.load();
      if (mute == 1.0f) {
        channel.muteL.store(0.0f);
        return 0.0f;
      }
      if (mute == 0.0f) {
        channel.muteL.store(1.0f);
        return 1.0f;
      }
      return 0.0f;
    }

    float toggleMuteR() {
      const float mute = channel.muteR.load();
      if (mute == 1.0f) {
        channel.muteR.store(0.0f);
        return 0.0f;
      }
      if (mute == 0.0f) {
        channel.muteR.store(1.0f);
        return 1.0f;
      }
      return 0.0f;
    }

    float getSolo() const {
      return channel.solo.load();
    }

    float getSoloL() const {
      return channel.soloL.load();
    }

    float getSoloR() const {
      return channel.soloR.load();
    }

    float toggleSolo() {
      const float solo = channel.solo.load();
      if (solo == 1.0f) {
        channel.solo.store(0.0f);
        return 0.0f;
      }
      if (solo == 0.0f) {
        channel.solo.store(1.0f);
        return 1.0f;
      }
      return 0.0f;
    }

    float toggleSoloL() {
      const float solo = channel.soloL.load();
      if (solo == 1.0f) {
        channel.soloL.store(0.0f);
        return 0.0f;
      }
      if (solo == 0.0f) {
        channel.soloL.store(1.0f);
        return 1.0f;
      }
      return 0.0f;
    }

    float toggleSoloR() {
      const float solo = channel.soloR.load();
      if (solo == 1.0f) {
        channel.soloR.store(0.0f);
        return 0.0f;
      }
      if (solo == 0.0f) {
        channel.soloR.store(1.0f);
        return 1.0f;
      }
      return 0.0f;
    }

    float getPan() const {
      return channel.pan.load();
    };

    bool setPan(float pan) {
      channel.pan.store(pan);
      return true;
    };

    float getPanL() const {
      return channel.panL.load();
    };

    bool setPanL(float pan) {
      channel.panL.store(pan);
      return true;
    };

    float getPanR() const {
      return channel.panR.load();
    };

    bool setPanR(float pan) {
      channel.panR.store(pan);
      return true;
    };

    bool addReplaceEffect(std::optional<EffectIndex> effectIdxOpt, const std::string& effectPath);
    Result loadEffect(const Db::Effect& effectEntity);
    Result setSampleRate(double sampleRate) const;
    Result setBlockSize(jack_nframes_t blockSize) const;

    size_t effectCount() const;
    void initEditorHosts(const std::vector<std::shared_ptr<Gui::VstWindow>>& vstWindows) const;
    void initEditorHost(EffectIndex effectIndex, std::shared_ptr<Gui::VstWindow> vstWindow) const;
    void terminateEditorHosts() const;

    bool removeEffect(EffectIndex effectIdx);

    AtomicStr getPluginName(const EffectIndex pluginIndex) const { return vst3Plugins.at(pluginIndex)->getName(); };

    Db::ChannelEntity toEntity() {
      return {
        id,
        index,
        name,
        channel.gain.load(),
        channel.mute.load(),
        channel.solo.load(),
        channel.pan.load(),
        channel.gainL.load(),
        channel.gainR.load(),
        channel.muteL.load(),
        channel.muteR.load(),
        channel.soloL.load(),
        channel.soloR.load(),
        channel.panL.load(),
        channel.panR.load(),
      };
    }
};

} // Effects
} // Audio
} // Gj


#endif //EFFECTCHANNEL_H
