//
// Created by ns on 11/11/24.
//

#ifndef PLAYBACK_H
#define PLAYBACK_H

#include "caf/actor_ostream.hpp"
#include "caf/actor_registry.hpp"
#include "caf/actor_system.hpp"
#include "caf/caf_main.hpp"
#include "caf/event_based_actor.hpp"

#include "./ActorIds.h"
#include "../messaging/atoms.h"
#include "../messaging/EnvelopeQtPtr.h"
#include "./AudioThread.h"

#include "../enums/PlayState.h"

using namespace caf;

namespace Gj {
namespace Act {

struct PlaybackTrait {

    using signatures = type_list<
                                  result<void>(strong_actor_ptr, tc_trig_play_a),
                                  result<void>(strong_actor_ptr, tc_trig_pause_a),
                                  result<void>(strong_actor_ptr, tc_trig_stop_a),
                                  result<void>(strong_actor_ptr, tc_trig_rw_a),
                                  result<void>(strong_actor_ptr, tc_trig_ff_a)
                       >;

};

using Playback = typed_actor<PlaybackTrait>;

struct PlaybackState {

     Playback::pointer self;
     Steinberg::Vst::AudioHost::App* vst3Host;

     PlaybackState(Playback::pointer self, strong_actor_ptr supervisor, Steinberg::Vst::AudioHost::App* vst3Host) :
          self(self)
        , vst3Host(vst3Host)
        {
           self->link_to(supervisor);
           self->system().registry().put(ActorIds::PLAYBACK, actor_cast<strong_actor_ptr>(self));
        }

     Playback::behavior_type make_behavior() {
       return {
           [this](strong_actor_ptr reply_to, tc_trig_play_a) {
             std::cout << "Playback : tc_trig_play_a : " << std::endl;

             auto audioThread = self->system().spawn(actor_from_state<AudioThreadState>, actor_cast<strong_actor_ptr>(self), vst3Host);
             self->anon_send(
                 audioThread,
                 audio_thread_init_a_v
             );

             actor replyToActor = actor_cast<actor>(reply_to);
             self->anon_send(
                 replyToActor,
                 actor_cast<strong_actor_ptr>(self),
                 true, // TODO: manage AudioThread and pass in result here
                 tc_trig_play_ar_v
             );
           },
           [this](strong_actor_ptr reply_to, tc_trig_pause_a) {
             std::cout << "Playback : tc_trig_pause_a : " << std::endl;

             actor replyToActor = actor_cast<actor>(reply_to);
             self->anon_send(
                 replyToActor,
                 actor_cast<strong_actor_ptr>(self),
                 true, // TODO: manage AudioThread and pass in result here
                 tc_trig_pause_ar_v
             );
           },
           [this](strong_actor_ptr reply_to, tc_trig_stop_a) {
             std::cout << "Playback : tc_trig_stop_a : " << std::endl;

             actor replyToActor = actor_cast<actor>(reply_to);
             self->anon_send(
                 replyToActor,
                 actor_cast<strong_actor_ptr>(self),
                 true, // TODO: manage AudioThread and pass in result here
                 tc_trig_stop_ar_v
             );
           },
           [this](strong_actor_ptr reply_to, tc_trig_rw_a) {
             std::cout << "Playback : tc_trig_rw_a : " << std::endl;

             actor replyToActor = actor_cast<actor>(reply_to);
             self->anon_send(
                 replyToActor,
                 actor_cast<strong_actor_ptr>(self),
                 true, // TODO: manage AudioThread and pass in result here
                 tc_trig_rw_ar_v
             );
           },
           [this](strong_actor_ptr reply_to, tc_trig_ff_a) {
             std::cout << "Playback : tc_trig_ff_a : " << std::endl;

             actor replyToActor = actor_cast<actor>(reply_to);
             self->anon_send(
                 replyToActor,
                 actor_cast<strong_actor_ptr>(self),
                 true, // TODO: manage AudioThread and pass in result here
                 tc_trig_ff_ar_v
             );
           },
       };
     };
};

} // Act
} // Gj

#endif //PLAYBACK_H