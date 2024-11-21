//
// Created by ns on 11/16/24.
//

#ifndef APPSTATEMANAGER_H
#define APPSTATEMANAGER_H

//
// Created by ns on 11/11/24.
//

#include "caf/actor_ostream.hpp"
#include "caf/actor_registry.hpp"
#include "caf/actor_system.hpp"
#include "caf/caf_main.hpp"
#include "caf/event_based_actor.hpp"

#include "./ActorIds.h"
#include "../messaging/atoms.h"
#include "../enums/PlayState.h"
#include "../AppState.h"

using namespace caf;

namespace Gs {
namespace Act {

struct AppStateManagerTrait {

    using signatures = type_list<
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
                                 result<void>(strong_actor_ptr, read_state_a)
                               >;

};

using AppStateManager = typed_actor<AppStateManagerTrait>;

struct AppStateManagerState {

     AppStateManager::pointer self;
     Gs::AppState appState;
     strong_actor_ptr playback;
     strong_actor_ptr display;

     AppStateManagerState(AppStateManager::pointer self, strong_actor_ptr supervisor) :
          self(self)
        , appState(Gs::AppState { Gs::PlayState::STOP } )
        {
           self->link_to(supervisor);
           self->system().registry().put(ActorIds::APP_STATE_MANAGER, actor_cast<strong_actor_ptr>(self));

           playback = self->system().registry().get(ActorIds::PLAYBACK);
           display = self->system().registry().get(ActorIds::DISPLAY);
        }

     AppStateManager::behavior_type make_behavior() {
       return {
           [this](strong_actor_ptr replyTo, read_state_a) {
             std::cout << "AppStateManager : read_state_a : " << std::endl;
             this->self->anon_send(
                 actor_cast<actor>(replyTo),
                 actor_cast<strong_actor_ptr>(self),
                 appState.toPacket(),
                 current_state_a_v
             );
           },
           [this](tc_trig_play_a) {
             std::cout << "AppStateManager : tc_trig_play_a : " << std::endl;

             this->self->anon_send(
                 actor_cast<actor>(playback),
                 actor_cast<strong_actor_ptr>(self),
                 tc_trig_play_a_v
             );
           },
           [this](strong_actor_ptr, bool success, tc_trig_play_ar) {
             std::cout << "AppStateManager : tc_trig_play_ar : " << std::endl;

             if (success) {
               appState = Gs::AppState::setPlayState(appState, Gs::PlayState::PLAY);
             } else {
               appState = Gs::AppState::setPlayState(appState, Gs::PlayState::STOP);
             }
             strong_actor_ptr displayPtr = self->system().registry().get(ActorIds::DISPLAY);
             this->self->anon_send(
               actor_cast<actor>(displayPtr),
               actor_cast<strong_actor_ptr>(self),
               hydrate_display_a_v
             );
           },
           [this](tc_trig_pause_a) {
             std::cout << "Gs::AppStateManager : tc_trig_pause_a : " << std::endl;

             this->self->anon_send(
                 actor_cast<actor>(playback),
                 actor_cast<strong_actor_ptr>(self),
                 tc_trig_pause_a_v
             );
           },
           [this](strong_actor_ptr, bool success, tc_trig_pause_ar) {
             std::cout << "Gs::AppStateManager : tc_trig_pause_ar : " << std::endl;

             if (success) {
               appState = Gs::AppState::setPlayState(appState, Gs::PlayState::PAUSE);
             } else {
               appState = Gs::AppState::setPlayState(appState, Gs::PlayState::STOP);
             }
           },
           [this](tc_trig_stop_a) {
             std::cout << "Gs::AppStateManager : tc_trig_stop_a : " << std::endl;

             this->self->anon_send(
                 actor_cast<actor>(playback),
                 actor_cast<strong_actor_ptr>(self),
                 tc_trig_stop_a_v
             );
           },
           [this](strong_actor_ptr, bool success, tc_trig_stop_ar) {
             std::cout << "Gs::AppStateManager : tc_trig_stop_ar : " << std::endl;
             appState = Gs::AppState::setPlayState(appState, Gs::PlayState::STOP);
           },
           [this](tc_trig_rw_a) {
             std::cout << "Gs::AppStateManager : tc_trig_rw_a : " << std::endl;

             this->self->anon_send(
                 actor_cast<actor>(playback),
                 actor_cast<strong_actor_ptr>(self),
                 tc_trig_rw_a_v
             );
           },
           [this](strong_actor_ptr, bool success, tc_trig_rw_ar) {
             std::cout << "Gs::AppStateManager : tc_trig_rw_ar : " << std::endl;

             if (success) {
               appState = Gs::AppState::setPlayState(appState, Gs::PlayState::RW);
             } else {
               appState = Gs::AppState::setPlayState(appState, Gs::PlayState::STOP);
             }
           },
           [this](tc_trig_ff_a) {
             std::cout << "Gs::AppStateManager : tc_trig_ff_a : " << std::endl;

             this->self->anon_send(
                 actor_cast<actor>(playback),
                 actor_cast<strong_actor_ptr>(self),
                 tc_trig_ff_a_v
             );
           },
           [this](strong_actor_ptr, bool success, tc_trig_ff_ar) {
             std::cout << "Gs::AppStateManager : tc_trig_ff_ar : " << std::endl;

             if (success) {
               appState = Gs::AppState::setPlayState(appState, Gs::PlayState::FF);
             } else {
               appState = Gs::AppState::setPlayState(appState, Gs::PlayState::STOP);
             }
           }
       };
     };
};

} // Act
} // Gs

#endif //APPSTATEMANAGER_H
