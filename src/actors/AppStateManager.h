//
// Created by ns on 11/16/24.
//

#ifndef APPSTATEMANAGER_H
#define APPSTATEMANAGER_H

//
// Created by ns on 11/11/24.
//
#include <optional>
#include <string>

#include "caf/actor_registry.hpp"
#include "caf/actor_system.hpp"
#include "caf/event_based_actor.hpp"

#include "../db/Dao.h"

#include "../Logging.h"
#include "./ActorIds.h"
#include "../messaging/atoms.h"
#include "../messaging/DecksState.h"
#include "../enums/PlayState.h"
#include "../AppState.h"
#include "../audio/AudioCore.h"
#include "../audio/mixer/Core.h"

using namespace caf;

namespace Gj {
namespace Act {
struct AppStateManagerTrait {
  using signatures = type_list<
    // Mixer
    result<void>(mix_add_effects_channel_a),
    result<void>(ChannelIndex, mix_remove_effects_channel_a),
    result<void>(ChannelIndex, PluginPath, mix_add_plugin_to_channel_a),
    result<void>(ChannelIndex, PluginIndex, PluginPath, mix_replace_plugin_on_channel_a),
    result<void>(ChannelIndex, PluginIndex, mix_remove_plugin_on_channel_a),
    result<void>(ChannelIndex, float, mix_set_channel_gain_a),

    // Transport control
    result<void>(tc_trig_play_a),
    result<void>(strong_actor_ptr, bool, tc_trig_play_ar),
    result<void>(tc_trig_pause_a),
    result<void>(strong_actor_ptr, bool, tc_trig_pause_ar),
    result<void>(tc_trig_stop_a),
    result<void>(strong_actor_ptr, bool, tc_trig_stop_ar),
    result<void>(tc_trig_rw_a),
    result<void>(strong_actor_ptr, bool, tc_trig_rw_ar),
    result<void>(tc_trig_ff_a),
    result<void>(strong_actor_ptr, bool, tc_trig_ff_ar),

    // Music Library
    result<void>(bool /* queuePlay */, DecksState, tc_trig_play_file_a),

    // Read state
    result<void>(strong_actor_ptr, read_state_a),

    // Hydrate state
    result<void>(hydrate_display_a)
  >;
};

using AppStateManager = typed_actor<AppStateManagerTrait>;

struct AppStateManagerState {
  AppStateManager::pointer self;
  AppState* gAppState;
  Audio::Mixer::Core* mixer;
  Audio::AudioCore* audioCore;
  Db::Dao* dao;
  strong_actor_ptr playback;
  strong_actor_ptr display;

  void hydrateStateToDisplay() {
    Logging::write(
      Info,
      "Act::AppStateManagerState::hydrateStateToDisplay",
      "Will send state to display"
    );

    strong_actor_ptr displayPtr = self->system().registry().get(DISPLAY);
    self->anon_send(
      actor_cast<actor>(displayPtr),
      actor_cast<strong_actor_ptr>(self),
      gAppState->toPacket(),
      current_state_a_v
    );
  };

  AppStateManagerState(
    AppStateManager::pointer self,
    strong_actor_ptr supervisor,
    AppState* gAppState,
    Audio::Mixer::Core* mixer,
    Db::Dao* dao,
    Audio::AudioCore* audioCore
  )
  : self(self)
    , gAppState(gAppState)
    , mixer(mixer)
    , audioCore(audioCore)
    , dao(dao) {
    self->link_to(supervisor);
    self->system().registry().put(APP_STATE_MANAGER, actor_cast<strong_actor_ptr>(self));

    playback = self->system().registry().get(PLAYBACK);
    display = self->system().registry().get(DISPLAY);
  }

  AppStateManager::behavior_type make_behavior() {
    return {
      [this](hydrate_display_a) {
        Logging::write(
          Info,
          "Act::AppStateManager::hydrate_display_a",
          "Hydrating state to display"
        );
        hydrateStateToDisplay();
      },
      [this](mix_add_effects_channel_a) {
        Logging::write(
          Info,
          "Act::AppStateManager::mix_add_effects_channel_a",
          "Received Add Mixer Effects Channel"
        );
        mixer->addEffectsChannel();
        hydrateStateToDisplay();
      },
      [this](const ChannelIndex channelIdx, mix_remove_effects_channel_a) {
        Logging::write(
          Info,
          "Act::AppStateManager::mix_remove_effects_channel_a",
          "Removing Mixer Effects Channel " + std::to_string(channelIdx)
        );
        mixer->removeEffectsChannel(channelIdx);
        hydrateStateToDisplay();
      },
      [this](const ChannelIndex channelIdx, PluginPath pluginPath, mix_add_plugin_to_channel_a) {
        Logging::write(
          Info,
          "Act::AppStateManager::mix_add_plugin_to_channel_a",
          "Adding Effect " + pluginPath + " on channel " + std::to_string(channelIdx)
        );
        // if (!mixer->addEffectToChannel(channelIdx, effectPath)) {
        //   Logging::write(
        //     Error,
        //     "Act::AppStateManager::mix_add_effects_channel_a",
        //     "Unable to add effect: " + effectPath + " to channel " + std::to_string(channelIdx)
        //   );
        // }
        hydrateStateToDisplay();
      },
      [this](const ChannelIndex channelIdx, const PluginIndex pluginIdx, PluginPath pluginPath,
             mix_replace_plugin_on_channel_a) {
        Logging::write(
          Info,
          "Act::AppStateManager::mix_replace_plugin_on_channel_a",
          "Replacing effect " + std::to_string(pluginIdx) + " on channel " + std::to_string(channelIdx) + " with " +
          pluginPath
        );
        // if (mixer->replacePluginOnChannel(channelIdx, pluginIdx, pluginPath) != OK) {
        //   Logging::write(
        //     Error,
        //     "Act::AppStateManager::mix_replace_plugin_on_channel_a",
        //     "Unable to add replace effect " + std::to_string(pluginIdx) + " on channel " + std::to_string(channelIdx) + " with " + pluginPath
        //   );
        // }
        hydrateStateToDisplay();
      },
      [this](const ChannelIndex channelIdx, const PluginIndex pluginIdx, mix_remove_plugin_on_channel_a) {
        Logging::write(
          Info,
          "Act::AppStateManager::mix_remove_plugin_on_channel_a",
          "Removing effect " + std::to_string(pluginIdx) + " on channel " + std::to_string(channelIdx)
        );
        // if (mixer->removePluginFromChannel(channelIdx, pluginIdx) != OK) {
        //   Logging::write(
        //     Error,
        //     "Act::AppStateManager::mix_remove_plugin_on_channel_a",
        //     "Unable to add remove effect " + std::to_string(pluginIdx) + " on channel " + std::to_string(channelIdx)
        //   );
        // }
        hydrateStateToDisplay();
      },
      [this](const ChannelIndex channelIdx, const float gain, mix_set_channel_gain_a) {
        if (mixer->setGainOnChannel(channelIdx, gain) != OK) {
          Logging::write(
            Error,
            "Act::AppStateManager::mix_set_channel_gain_a",
            "Unable to set gain of " + std::to_string(gain) + " on channel " + std::to_string(channelIdx)
          );
        }
        hydrateStateToDisplay();
      },
      [this](strong_actor_ptr replyTo, read_state_a) {
        Logging::write(
          Info,
          "Act::AppStateManager::read_state_a",
          "Received read state message"
        );
        self->anon_send(
          actor_cast<actor>(replyTo),
          actor_cast<strong_actor_ptr>(self),
          gAppState->toPacket(),
          current_state_a_v
        );
      },
      [this](tc_trig_play_a) {
        Logging::write(
          Info,
          "Act::AppStateManager::tc_trig_play_a",
          "Received TC Play Trig"
        );

        self->anon_send(
          actor_cast<actor>(playback),
          actor_cast<strong_actor_ptr>(self),
          tc_trig_play_a_v
        );
      },
      [this](strong_actor_ptr, const bool success, tc_trig_play_ar) {
        Logging::write(
          Info,
          "Act::AppStateManager::tc_trig_play_ar",
          "Received TC Play Trig Response - status " + std::to_string(success)
        );

        if (success) {
          gAppState->setPlayState(PLAY);
        } else {
          gAppState->setPlayState(STOP);
        }

        Logging::write(
          Info,
          "Act::AppStateManager::tc_trig_play_ar",
          "Updated state. Will HydrateStateToDisplay"
        );

        hydrateStateToDisplay();
      },
      [this](tc_trig_pause_a) {
        Logging::write(
          Info,
          "Act::AppStateManager::tc_trig_pause_a",
          "Received TC Pause Trig"
        );

        gAppState->setPlayState(PAUSE);

        self->anon_send(
          actor_cast<actor>(playback),
          actor_cast<strong_actor_ptr>(self),
          tc_trig_pause_a_v
        );
      },
      [this](strong_actor_ptr, const bool success, tc_trig_pause_ar) {
        Logging::write(
          Info,
          "Act::AppStateManager::tc_trig_pause_a",
          "Received TC Pause Trig Response - status: " + std::to_string(success)
        );

        if (success) {
          gAppState->setPlayState(PAUSE);
        } else {
          gAppState->setPlayState(STOP);
        }

        Logging::write(
          Info,
          "Act::AppStateManager::tc_trig_play_ar",
          "Updated state. Will HydrateStateToDisplay"
        );

        hydrateStateToDisplay();
      },
      [this](tc_trig_stop_a) {
        Logging::write(
          Info,
          "Act::AppStateManager::tc_trig_stop_a",
          "Received TC Stop Trig"
        );

        self->anon_send(
          actor_cast<actor>(playback),
          actor_cast<strong_actor_ptr>(self),
          tc_trig_stop_a_v
        );
      },
      [this](strong_actor_ptr, const bool success, tc_trig_stop_ar) {
        Logging::write(
          Info,
          "Act::AppStateManager::tc_trig_stop_ar",
          "Received TC Stop Trig Response - status: " + std::to_string(success)
        );

        gAppState->setPlayState(STOP);

        Logging::write(
          Info,
          "Act::AppStateManager::tc_trig_stop_ar",
          "Updated state. Will HydrateStateToDisplay"
        );

        hydrateStateToDisplay();
      },
      [this](tc_trig_rw_a) {
        Logging::write(
          Info,
          "Act::AppStateManager::tc_trig_rw_a",
          "Received TC RW Trig"
        );

        self->anon_send(
          actor_cast<actor>(playback),
          actor_cast<strong_actor_ptr>(self),
          tc_trig_rw_a_v
        );
      },
      [this](strong_actor_ptr, const bool success, tc_trig_rw_ar) {
        Logging::write(
          Info,
          "Act::AppStateManager::tc_trig_rw_a",
          "Received TC RW Trig Response - status: " + std::to_string(success)
        );

        if (success) {
          gAppState->setPlayState(RW);
        } else {
          gAppState->setPlayState(STOP);
        }

        Logging::write(
          Info,
          "Act::AppStateManager::tc_trig_rw_ar",
          "Updated state. Will HydrateStateToDisplay"
        );

        hydrateStateToDisplay();
      },
      [this](tc_trig_ff_a) {
        Logging::write(
          Info,
          "Act::AppStateManager::tc_trig_ff_a",
          "Received TC FF Trig"
        );

        self->anon_send(
          actor_cast<actor>(playback),
          actor_cast<strong_actor_ptr>(self),
          tc_trig_ff_a_v
        );
      },
      [this](strong_actor_ptr, const bool success, tc_trig_ff_ar) {
        Logging::write(
          Info,
          "Act::AppStateManager::tc_trig_ff_a",
          "Received TC FF Trig Response - status: " + std::to_string(success)
        );

        if (success) {
          gAppState->setPlayState(FF);
        } else {
          gAppState->setPlayState(STOP);
        }

        Logging::write(
          Info,
          "Act::AppStateManager::tc_trig_ff_ar",
          "Updated state. Will HydrateStateToDisplay"
        );

        hydrateStateToDisplay();
      },
      [this](const bool queuePlay, const DecksState decksState, tc_trig_play_file_a) {
        const int audioFileId = decksState.audioFileIds[decksState.currentDeckIdx];
        Logging::write(
          Info,
          "Act::AppStateManager::tc_trig_play_file_a",
          "Received TC Trig Play File AudioFileId: " + std::to_string(audioFileId)
        );

        gAppState->queuePlay = queuePlay;
        Db::DecoratedAudioFile decoratedAudioFiles[Audio::AUDIO_CORE_DECK_COUNT];
        for (int i = 0; i < Audio::AUDIO_CORE_DECK_COUNT; i++) {
          const std::optional<Db::DecoratedAudioFile> decoratedAudioFile =
              dao->audioFileRepository.findDecoratedAudioFileById(decksState.audioFileIds[i]);
          if (decoratedAudioFile) {
            decoratedAudioFiles[i] = decoratedAudioFile.value();
          } else {
            Logging::write(
              Error,
              "Act::AppStateManager::tc_trig_play_file_a",
              "Unable to load DecoratedAudioFile Id: " + std::to_string(audioFileId)
            );
          }
        }

        for (int i = 0; i < Audio::AUDIO_CORE_DECK_COUNT; i++) {
          if (audioCore->addCassetteFromDecoratedAudioFileAtIdx(decoratedAudioFiles[i], i) == ERROR) {
            Logging::write(
              Error,
              "Act::AppStateManager::tc_trig_play_file_a",
              "Unable to addCassette from DecoratedAudioFile filePath: " + decoratedAudioFiles[i].audioFile.filePath
            );
            return;
          }
        }
        audioCore->deckIndex = decksState.currentDeckIdx;
        audioCore->deckIndexNext = decksState.currentDeckIdx;
        gAppState->setCurrentlyPlaying(audioCore->currentDeck().decoratedAudioFile.value());

        Logging::write(
          Info,
          "Act::AppStateManager::tc_trig_play_file_a",
          "Initiated file play. Will HydrateStateToDisplay"
        );

        self->anon_send(
          actor_cast<actor>(playback),
          actor_cast<strong_actor_ptr>(self),
          tc_trig_play_a_v
        );

        hydrateStateToDisplay();
      }
    };
  };
};
} // Act
} // Gj

#endif //APPSTATEMANAGER_H
