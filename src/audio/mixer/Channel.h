//
// Created by ns on 1/12/25.
//

#ifndef GJAUDIOMIXERCHANNEL_H
#define GJAUDIOMIXERCHANNEL_H

#include <atomic>
#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <mutex>
#include <vector>

#include "../plugins/vst3/Plugin.h"
#include "ChannelSettings.h"
#include "ChannelProcessData.h"
#include "../../state/Core.h"
#include "../../enums/Result.h"
#include "../../db/entity/mixer/Plugin.h"
#include "../Logging.h"
#include "../JackClient.h"
#include "../../types/AtomicStr.h"
#include "../../types/Types.h"

namespace Gj {
namespace Audio {
namespace Mixer {
class Channel {
  // NOTE:
  // fx0:inputBuffers->buffersA, fx1:buffersA->buffersB, fx2:buffersB->buffersA, fx3:buffersA->buffersB, ...
  State::Core* stateCore;
  std::shared_ptr<JackClient> jackClient;
  ID id{0};
  ChannelIndex index;
  AtomicStr name{"Channel"};

  std::optional<Plugins::Vst3::Plugin*> plugins[MAX_PLUGINS_PER_CHANNEL] = {};
  std::vector<Plugins::Vst3::Plugin*> pluginsToDelete = {};

public:
  Result deletePluginsToDelete() {
    Logging::write(
      Info,
      "Audio::Channel::deletePluginsToDelete",
      "Deleting plugins marked for deletion."
    );
    for (const auto plugin: pluginsToDelete) {
      delete plugin;
    }
    pluginsToDelete.clear();
    return OK;
  }

  ChannelSettings settings;

  Channel(
    State::Core* stateCore,
    std::shared_ptr<JackClient> jackClient,
    ChannelIndex index
  );

  Channel(
    State::Core* stateCore,
    std::shared_ptr<JackClient> jackClient,
    const Db::ChannelEntity& channelEntity
  );

  ~Channel();

  Result setupProcessing() const {
    Result res = OK;

    for (const auto& plugin: plugins) {
      if (plugin.has_value() && plugin.value()->setupProcessing() != OK)
        res = ERROR;
    }

    return res;
  }

  [[nodiscard]]
  ChannelIndex getIndex() const { return index; }

  [[nodiscard]]
  std::optional<Plugins::Vst3::Plugin*> getPluginAtIdx(const PluginIndex idx) const {
    if (idx > MAX_PLUGINS_PER_CHANNEL)
      return std::nullopt;

    return plugins[idx];
  };

  float getGain() const {
    return settings.gain.load();
  };

  bool setGain(const float gain) {
    settings.gain.store(gain);
    return true;
  };

  float getGainL() const {
    return settings.gainL.load();
  };

  bool setGainL(float gain) {
    settings.gainL.store(gain);
    return true;
  };

  float getGainR() const {
    return settings.gainR.load();
  };

  bool setGainR(float gain) {
    settings.gainR.store(gain);
    return true;
  };

  float getMute() const {
    return settings.mute.load();
  }

  float getMuteL() const {
    return settings.muteL.load();
  }

  float getMuteR() const {
    return settings.muteR.load();
  }

  float toggleMute() {
    const float mute = settings.mute.load();
    if (mute == 1.0f) {
      settings.mute.store(0.0f);
      return 0.0f;
    }
    if (mute == 0.0f) {
      settings.mute.store(1.0f);
      return 1.0f;
    }
    return 0.0f;
  }

  float toggleMuteL() {
    const float mute = settings.muteL.load();
    if (mute == 1.0f) {
      settings.muteL.store(0.0f);
      return 0.0f;
    }
    if (mute == 0.0f) {
      settings.muteL.store(1.0f);
      return 1.0f;
    }
    return 0.0f;
  }

  float toggleMuteR() {
    const float mute = settings.muteR.load();
    if (mute == 1.0f) {
      settings.muteR.store(0.0f);
      return 0.0f;
    }
    if (mute == 0.0f) {
      settings.muteR.store(1.0f);
      return 1.0f;
    }
    return 0.0f;
  }

  float getSolo() const {
    return settings.solo.load();
  }

  float getSoloL() const {
    return settings.soloL.load();
  }

  float getSoloR() const {
    return settings.soloR.load();
  }

  float toggleSolo() {
    const float solo = settings.solo.load();
    if (solo == 1.0f) {
      settings.solo.store(0.0f);
      return 0.0f;
    }
    if (solo == 0.0f) {
      settings.solo.store(1.0f);
      return 1.0f;
    }
    return 0.0f;
  }

  float toggleSoloL() {
    const float solo = settings.soloL.load();
    if (solo == 1.0f) {
      settings.soloL.store(0.0f);
      return 0.0f;
    }
    if (solo == 0.0f) {
      settings.soloL.store(1.0f);
      return 1.0f;
    }
    return 0.0f;
  }

  float toggleSoloR() {
    const float solo = settings.soloR.load();
    if (solo == 1.0f) {
      settings.soloR.store(0.0f);
      return 0.0f;
    }
    if (solo == 0.0f) {
      settings.soloR.store(1.0f);
      return 1.0f;
    }
    return 0.0f;
  }

  float getPan() const {
    return settings.pan.load();
  };

  bool setPan(float pan) {
    settings.pan.store(pan);
    return true;
  };

  float getPanL() const {
    return settings.panL.load();
  };

  bool setPanL(float pan) {
    settings.panL.store(pan);
    return true;
  };

  float getPanR() const {
    return settings.panR.load();
  };

  bool setPanR(float pan) {
    settings.panR.store(pan);
    return true;
  };

  std::optional<PluginIndex> firstOpenPluginIndex() const;

  Result addReplacePlugin(std::optional<PluginIndex> pluginIdxOpt, const PluginPath& pluginPath);

  Result loadPlugin(const Db::Plugin& pluginEntity);

  Result setSampleRate(double sampleRate);

  Result setBlockSize(jack_nframes_t blockSize);

  PluginIndex pluginCount();

  Result initEditorHosts(const std::vector<std::shared_ptr<Gui::Mixer::VstWindow> >& vstWindows);

  Result initEditorHost(PluginIndex pluginIndex, std::shared_ptr<Gui::Mixer::VstWindow> vstWindow) const;

  Result terminateEditorHosts();

  Result removePlugin(PluginIndex pluginIdx);

  AtomicStr getPluginName(const PluginIndex pluginIndex) const {
    const auto plugin = getPluginAtIdx(pluginIndex);
    if (!plugin)
      return AtomicStr(" - ");
    return plugin.value()->getName();
  };

  template<typename F>
  Result forEachPlugin(F&& func) {
    // run against all valid plugins
    bool warning = false;
    for (PluginIndex pluginIndex = 0; pluginIndex < MAX_PLUGINS_PER_CHANNEL; ++pluginIndex) {
      if (!plugins[pluginIndex] || plugins[pluginIndex].value() == nullptr)
        continue;

      try {
        func(plugins[pluginIndex].value(), pluginIndex);
      } catch (...) {
        Logging::write(
          Error,
          "Audio::Mixer::Channel::forEachPlugin",
          "Error during lambda function for pluginIdx: " +
          std::to_string(pluginIndex)
        );
        warning = true;
      }
    }
    return warning ? WARNING : OK;
  }

  template<typename F>
  Result forEachPluginSlot(F&& func) {
    // run against every possible plugin slot
    bool warning = false;
    for (PluginIndex pluginIndex = 0; pluginIndex < MAX_PLUGINS_PER_CHANNEL; ++pluginIndex) {
      try {
        func(plugins[pluginIndex], pluginIndex);
      } catch (...) {
        Logging::write(
          Error,
          "Audio::Mixer::Channel::forEachPlugin",
          "Error during lambda function for pluginIdx: " +
          std::to_string(pluginIndex)
        );
        warning = true;
      }
    }
    return warning ? WARNING : OK;
  }

  Db::ChannelEntity toEntity() {
    return {
      id,
      index,
      name,
      settings.gain.load(),
      settings.mute.load(),
      settings.solo.load(),
      settings.pan.load(),
      settings.gainL.load(),
      settings.gainR.load(),
      settings.muteL.load(),
      settings.muteR.load(),
      settings.soloL.load(),
      settings.soloR.load(),
      settings.panL.load(),
      settings.panR.load(),
    };
  }
};
} // Mixer
} // Audio
} // Gj


#endif //GJAUDIOMIXERCHANNEL_H
