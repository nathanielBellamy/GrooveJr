//
// Created by ns on 11/22/24.
//

#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H

#include "caf/actor_ostream.hpp"
#include "caf/actor_registry.hpp"
#include "caf/actor_system.hpp"
#include "caf/caf_main.hpp"
#include "caf/event_based_actor.hpp"

#include "../messaging/atoms.h"
#include "./AppStateManager.h"
#include "./Playback.h"
#include "../state/Core.h"

#include "../audio/AudioCore.h"
#include "../audio/AudioPlayer.h"
#include "../audio/mixer/Core.h"
#include "../audio/Cassette.h"
#include "../Logging.h"
#include "../enums/Result.h"

using namespace caf;

namespace Gj {
namespace Act {
struct AudioThreadTrait {
  using signatures = type_list<
    result<void>(audio_thread_init_a)
  >;
};

using AudioThread = typed_actor<AudioThreadTrait>;

struct AudioThreadState {
  static long id;

  AudioThread::pointer self;
  State::Core* stateCore;
  Audio::Mixer::Core* mixer;
  Audio::AudioCore* audioCore;

  AudioThreadState(
    AudioThread::pointer self,
    strong_actor_ptr supervisor,
    State::Core* stateCore,
    Audio::Mixer::Core* mixer,
    Audio::AudioCore* audioCore
  ) : self(self)
      , stateCore(stateCore)
      , mixer(mixer)
      , audioCore(audioCore) {
    self->link_to(supervisor);
  }

  AudioThread::behavior_type make_behavior() {
    return {
      [this](audio_thread_init_a) {
        Logging::write(
          Info,
          "Act::AudioThread::audio_thread_init_a",
          "Instantiating AudioPlayer"
        );
        try {
          if (Audio::AudioPlayer audioPlayer(self->system(), audioCore, mixer, stateCore); audioPlayer.run() == ERROR)
            Logging::write(
              Error,
              "Act::AudioThread::audio_thread_init_a",
              "An error occurred during playback."
            );
        } catch (std::runtime_error& e) {
          Logging::write(
            Error,
            "Act::AudioThread::audio_thread_init_a",
            "Error creating AudioPlayer or during AudioPlayer::run: " + std::string(e.what())
          );
        }

        stateCore->setAudioRunning(false);
        Logging::write(
          Info,
          "Act::AudioThread::audio_thread_init_a",
          "AudioPlayer run complete."
        );
      },
    };
  };
};
} // Act
} // Gj

#endif //AUDIOTHREAD_H
