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

#include "../Logging.h"
#include "./ActorIds.h"
#include "../messaging/atoms.h"
#include "../messaging/EnvelopeQtPtr.h"
#include "./AudioThread.h"

#include "../state/Core.h"
#include "../audio/mixer/Core.h"
#include "../audio/AudioCore.h"
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

  State::Core* stateCore;
  Audio::Mixer::Core* mixer;
  Audio::AudioCore* audioCore;

  PlaybackState(
    Playback::pointer self,
    strong_actor_ptr supervisor,
    State::Core* stateCore,
    Audio::Mixer::Core* mixer,
    Audio::AudioCore* audioCore)
  : self(self)
    , audioThread(nullptr)
    , stateCore(stateCore)
    , mixer(mixer)
    , audioCore(audioCore) {
    self->link_to(supervisor);
    self->system().registry().put(PLAYBACK, actor_cast<strong_actor_ptr>(self));
  }

  void clearAudioThread() {
    if (audioThread != nullptr) {
      Logging::write(
        Info,
        "Act::Playback::clearAudioThread",
        "Clearing audio thread"
      );
      audioThread->quit();
      audioThread = nullptr;
      stateCore->setAudioRunning(false);
      mixer->safeDeleteOldPlugins();
    }
  }

  Playback::behavior_type make_behavior() {
    return {
      [this](strong_actor_ptr reply_to, tc_trig_play_a) {
        Logging::write(
          Info,
          "Act::Playback::tc_trig_play_a",
          "Received TC Trig Play"
        );

        if (!audioCore->currentDeck().hasValidCassetteLoaded()) {
          Logging::write(
            Warning,
            "Act::Playback::tc_trig_play_a",
            "Attempting to play without Valid Cassette Loaded."
          );
          return;
        }

        const PlayState playState = Audio::ThreadStatics::getPlayState();
        if (playState == PLAY)
          return;

        if (playState == STOP) {
          Audio::ThreadStatics::setPlayState(STOP);
          clearAudioThread();
        }

        bool success = true;
        Audio::ThreadStatics::setPlayState(PLAY);
        if (audioThread == nullptr) {
          auto audioThreadActor = self->system().spawn(
            actor_from_state<AudioThreadState>,
            actor_cast<strong_actor_ptr>(self),
            stateCore,
            mixer,
            audioCore
          );
          audioThread = actor_cast<AudioThread::pointer>(audioThreadActor);
          self->anon_send(
            audioThreadActor,
            audio_thread_init_a_v
          );
        }

        const actor replyToActor = actor_cast<actor>(reply_to);
        self->anon_send(
          replyToActor,
          actor_cast<strong_actor_ptr>(self),
          success,
          tc_trig_play_ar_v
        );
      },
      [this](strong_actor_ptr reply_to, tc_trig_pause_a) {
        Logging::write(
          Info,
          "Act::Playback::tc_trig_pause_a",
          "Received TC Pause Trig"
        );
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
        Logging::write(
          Info,
          "Act::Playback::tc_trig_stop_a",
          "Received TC Stop Trig"
        );

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
        Logging::write(
          Info,
          "Act::Playback::tc_trig_rw_a",
          "Received TC RW Trig"
        );

        Audio::ThreadStatics::setPlayState(RW);
        actor replyToActor = actor_cast<actor>(reply_to);
        self->anon_send(
          replyToActor,
          actor_cast<strong_actor_ptr>(self),
          true,
          tc_trig_rw_ar_v
        );
      },
      [this](strong_actor_ptr reply_to, tc_trig_ff_a) {
        Logging::write(
          Info,
          "Act::Playback::tc_trig_ff_a",
          "Received TC FF Trig"
        );

        Audio::ThreadStatics::setPlayState(FF);
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
