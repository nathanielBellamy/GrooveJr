//
// Created by ns on 11/16/24.
//

#ifndef APPSTATEMANAGER_H
#define APPSTATEMANAGER_H

//
// Created by ns on 11/11/24.
//
#include <string>

#include "caf/actor_ostream.hpp"
#include "caf/actor_registry.hpp"
#include "caf/actor_system.hpp"
#include "caf/caf_main.hpp"
#include "caf/event_based_actor.hpp"

#include "../Logging.h"
#include "./ActorIds.h"
#include "../messaging/atoms.h"
#include "../enums/PlayState.h"
#include "../AppState.h"
#include "../audio/Mixer.h"

using namespace caf;

namespace Gj {
namespace Act {

struct AppStateManagerTrait {

    using signatures = type_list<
                                 // Mixer
                                 result<void>(mix_add_effects_channel_a),
                                 result<void>(int, std::string, mix_add_effect_to_channel_a),

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

                                 // Read state
                                 result<void>(strong_actor_ptr, read_state_a)
                               >;
};

using AppStateManager = typed_actor<AppStateManagerTrait>;

struct AppStateManagerState {

     AppStateManager::pointer self;
     AppState* gAppState;
     Audio::Mixer* mixer;
     AppState appState;
     strong_actor_ptr playback;
     strong_actor_ptr display;

     void hydrateStateToDisplay() {
         Logging::write(
           Info,
           "AppStateManagerState::hydrateStateToDisplay",
           "Will send state to display"
         );

         strong_actor_ptr displayPtr = self->system().registry().get(DISPLAY);
         self->anon_send(
           actor_cast<actor>(displayPtr),
           actor_cast<strong_actor_ptr>(self),
           appState.toPacket(),
           current_state_a_v
         );
     };

     AppStateManagerState(AppStateManager::pointer self, strong_actor_ptr supervisor, AppState* gAppState, Audio::Mixer* mixer)
        : self(self)
        , gAppState(gAppState)
        , mixer(mixer)
        , appState(AppState { gAppState->audioFramesPerBuffer, gAppState->playState } )
        {
           self->link_to(supervisor);
           self->system().registry().put(APP_STATE_MANAGER, actor_cast<strong_actor_ptr>(self));

           playback = self->system().registry().get(PLAYBACK);
           display = self->system().registry().get(DISPLAY);
        }

     AppStateManager::behavior_type make_behavior() {
       return {
           [this](mix_add_effects_channel_a) {
             Logging::write(
               Info,
               "Act::AppStateManager::mix_add_effects_channel_a",
               "Received Add Mixer Effects Channel"
             );
             mixer->addEffectsChannel();
           },
           [this](const int channelIdx, std::string effectPath, mix_add_effect_to_channel_a) {
             Logging::write(
               Info,
               "Act::AppStateManager::mix_add_effects_channel_a",
               "Received Add Mixer Effects Channel"
             );
             if (!mixer->addEffectToChannel(channelIdx, effectPath)) {
               Logging::write(
                 Error,
                 "EffectsChannel::addEffectAction",
                 "Unable to add effect: " + effectPath + " to channel " + std::to_string(channelIdx)
               );
             }
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
                 appState.toPacket(),
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
           [this](strong_actor_ptr, bool success, tc_trig_play_ar) {
             Logging::write(
               Info,
               "Act::AppStateManager::tc_trig_play_ar",
               "Received TC Play Trig Response - status " + std::to_string(success)
             );

             if (success) {
               appState = AppState::setPlayState(appState, PLAY);
             } else {
               appState = AppState::setPlayState(appState, STOP);
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

             appState = AppState::setPlayState(appState, PAUSE);

             self->anon_send(
                 actor_cast<actor>(playback),
                 actor_cast<strong_actor_ptr>(self),
                 tc_trig_pause_a_v
             );
           },
           [this](strong_actor_ptr, bool success, tc_trig_pause_ar) {
             Logging::write(
               Info,
               "Act::AppStateManager::tc_trig_pause_a",
               "Received TC Pause Trig Response - status: " + std::to_string(success)
             );

             if (success) {
               appState = AppState::setPlayState(appState, PAUSE);
             } else {
               appState = AppState::setPlayState(appState, STOP);
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
           [this](strong_actor_ptr, bool success, tc_trig_stop_ar) {
             Logging::write(
               Info,
               "Act::AppStateManager::tc_trig_stop_ar",
               "Received TC Stop Trig Response - status: " + std::to_string(success)
             );

             appState = AppState::setPlayState(appState, STOP);

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
           [this](strong_actor_ptr, bool success, tc_trig_rw_ar) {
             Logging::write(
               Info,
               "Act::AppStateManager::tc_trig_rw_a",
               "Received TC RW Trig Response - status: " + std::to_string(success)
             );

             if (success) {
               appState = AppState::setPlayState(appState, RW);
             } else {
               appState = AppState::setPlayState(appState, STOP);
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
           [this](strong_actor_ptr, bool success, tc_trig_ff_ar) {
             Logging::write(
               Info,
               "Act::AppStateManager::tc_trig_ff_a",
               "Received TC FF Trig Response - status: " + std::to_string(success)
             );

             if (success) {
               appState = AppState::setPlayState(appState, FF);
             } else {
               appState = AppState::setPlayState(appState, STOP);
             }

             Logging::write(
               Info,
               "Act::AppStateManager::tc_trig_ff_ar",
               "Updated state. Will HydrateStateToDisplay"
             );

             hydrateStateToDisplay();
           }
       };
     };
};

} // Act
} // Gj

#endif //APPSTATEMANAGER_H
