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

#include "../AppState.h"
#include "../audio/Mixer.h"
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
     AudioThread::pointer audioThread;

     AppState* gAppState;
     Audio::Mixer* mixer;

     PlaybackState(Playback::pointer self, strong_actor_ptr supervisor, AppState* gAppState, Audio::Mixer* mixer) :
          self(self)
        , mixer(mixer)
        , audioThread(nullptr)
        {
           self->link_to(supervisor);
           self->system().registry().put(ActorIds::PLAYBACK, actor_cast<strong_actor_ptr>(self));
        }

    void clearAudioThread() {
        if (audioThread != nullptr) {
            std::cout << "Playback : non-null audioThread : " << std::endl;
            audioThread->quit();
            audioThread = nullptr;
        }
    }

     Playback::behavior_type make_behavior() {
       return {
           [this](strong_actor_ptr reply_to, tc_trig_play_a) {
             std::cout << "Playback : tc_trig_play_a : " << std::endl;

             const PlayState playState = Audio::ThreadStatics::getPlayState();
             if (playState == PLAY)
               return;

             if (playState == STOP) {
               Audio::ThreadStatics::setPlayState(STOP);
               clearAudioThread();
             }

             bool success = true;
             Audio::ThreadStatics::setPlayState(PLAY);
             const char* filePath = Audio::ThreadStatics::getFilePath();
             if ( filePath != nullptr ) {
               if ( audioThread == nullptr ) {
                   auto audioThreadActor = self->system().spawn(
                     actor_from_state<AudioThreadState>,
                     actor_cast<strong_actor_ptr>(self),
                     gAppState,
                     mixer
                   );
                   audioThread = actor_cast<AudioThread::pointer>(audioThreadActor);
                   self->anon_send(
                       audioThreadActor,
                       audio_thread_init_a_v
                   );
               }
             }

             actor replyToActor = actor_cast<actor>(reply_to);
             self->anon_send(
                 replyToActor,
                 actor_cast<strong_actor_ptr>(self),
                 success,
                 tc_trig_play_ar_v
             );
           },
           [this](strong_actor_ptr reply_to, tc_trig_pause_a) {
             std::cout << "Playback : tc_trig_pause_a : " << std::endl;
             Audio::ThreadStatics::setPlayState(PAUSE);

             clearAudioThread();
             const actor replyToActor = actor_cast<actor>(reply_to);
             self->anon_send(
                 replyToActor,
                 actor_cast<strong_actor_ptr>(self),
                 true,
                 tc_trig_pause_ar_v
             );
           },
           [this](strong_actor_ptr reply_to, tc_trig_stop_a) {
             std::cout << "Playback : tc_trig_stop_a : " << std::endl;

             clearAudioThread();
             Audio::ThreadStatics::setPlayState(STOP);
             const actor replyToActor = actor_cast<actor>(reply_to);
             self->anon_send(
                 replyToActor,
                 actor_cast<strong_actor_ptr>(self),
                 true,
                 tc_trig_stop_ar_v
             );
           },
           [this](strong_actor_ptr reply_to, tc_trig_rw_a) {
             std::cout << "Playback : tc_trig_rw_a : " << std::endl;

             Gj::Audio::ThreadStatics::setPlayState(Gj::PlayState::RW);
             actor replyToActor = actor_cast<actor>(reply_to);
             self->anon_send(
                 replyToActor,
                 actor_cast<strong_actor_ptr>(self),
                 true,
                 tc_trig_rw_ar_v
             );
           },
           [this](strong_actor_ptr reply_to, tc_trig_ff_a) {
             std::cout << "Playback : tc_trig_ff_a : " << std::endl;

             Audio::ThreadStatics::setPlayState(Gj::PlayState::FF);
             const actor replyToActor = actor_cast<actor>(reply_to);
             self->anon_send(
                 replyToActor,
                 actor_cast<strong_actor_ptr>(self),
                 true,
                 tc_trig_ff_ar_v
             );
           },
       };
     };
};

} // Act
} // Gj

#endif //PLAYBACK_H
