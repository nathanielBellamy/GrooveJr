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

#include "../audio/effects/vst3/Plugin.h"
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
     std::vector<Audio::Effects::Vst3::Plugin*>& vst3Plugins;

     PlaybackState(Playback::pointer self, strong_actor_ptr supervisor, std::vector<Audio::Effects::Vst3::Plugin*>& vst3Plugins) :
          self(self)
        , vst3Plugins(vst3Plugins)
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

             Gj::PlayState playState = Gj::Audio::ThreadStatics::getPlayState();
             if (playState == Gj::PlayState::PLAY)
               return;

             if (playState == Gj::PlayState::STOP) {
               Gj::Audio::ThreadStatics::setPlayState(Gj::PlayState::STOP);
               clearAudioThread();
             }

             bool success = true;
             Gj::Audio::ThreadStatics::setPlayState(Gj::PlayState::PLAY);
             const char* filePath = Gj::Audio::ThreadStatics::getFilePath();
             if ( filePath != nullptr ) {
               if ( audioThread == nullptr ) {
                   auto audioThreadActor = self->system().spawn(actor_from_state<AudioThreadState>, actor_cast<strong_actor_ptr>(self), vst3Plugins);
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
             Gj::Audio::ThreadStatics::setPlayState(Gj::PlayState::PAUSE);

             clearAudioThread();
             actor replyToActor = actor_cast<actor>(reply_to);
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
             Gj::Audio::ThreadStatics::setPlayState(Gj::PlayState::STOP);
             actor replyToActor = actor_cast<actor>(reply_to);
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

             Gj::Audio::ThreadStatics::setPlayState(Gj::PlayState::FF);
             actor replyToActor = actor_cast<actor>(reply_to);
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
