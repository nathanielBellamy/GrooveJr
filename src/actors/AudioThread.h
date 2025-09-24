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
#include "../AppState.h"

#include "../audio/AudioCore.h"
#include "../audio/AudioPlayer.h"
#include "../audio/Mixer.h"
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
     AppState* gAppState;
     Audio::Mixer* mixer;
     Audio::AudioCore* audioCore;

     AudioThreadState(
       AudioThread::pointer self,
       strong_actor_ptr supervisor,
       AppState* gAppState,
       Audio::Mixer* mixer,
       Audio::AudioCore* audioCore
    ) :
         self(self)
       , gAppState(gAppState)
       , mixer(mixer)
       , audioCore(audioCore)
       {
           self->link_to(supervisor);
       }

     AudioThread::behavior_type make_behavior() {
       return {
           [this](audio_thread_init_a) {
             Logging::write(
               Info,
               "Act::AudioThread::audio_thread_init_a",
               "Instantiating Cassette"
             );
             try {
               // WIP
               // - loading cassettes on decks
               const char* filePath1 = "/Users/ns/Music/Amy Winehouse/Back to Black/Amy Winehouse - Back to Black (2006) [FLAC]/05 Back To Black.flac";
               const char* filePath2 = "/Users/ns/Music/Amy Winehouse/Back to Black/Amy Winehouse - Back to Black (2006) [FLAC]/06 Love Is A Losing Game.flac";
               const char* filePath3 = "/Users/ns/Music/Amy Winehouse/Back to Black/Amy Winehouse - Back to Black (2006) [FLAC]/07 Tears Dry On Their Own.flac";
               if (audioCore->addCassetteFromFilePath(filePath1) == ERROR)
                 Logging::write(
                   Error,
                   "Act::AudioThread::audio_thread_init_a",
                   "Unable to add Cassette1 to audioCore"
                  );
               if (audioCore->addCassetteFromFilePath(filePath2) == ERROR)
                 Logging::write(
                   Error,
                   "Act::AudioThread::audio_thread_init_a",
                   "Unable to add Cassette2 to audioCore"
                  );
               if (audioCore->addCassetteFromFilePath(filePath3) == ERROR)
                 Logging::write(
                   Error,
                   "Act::AudioThread::audio_thread_init_a",
                   "Unable to add Cassette3 to audioCore"
                  );
               audioCore->setDeckIndex(0);
               if (Audio::AudioPlayer audioPlayer (audioCore, mixer, gAppState); audioPlayer.play() == ERROR)
                 Logging::write(
                   Error,
                   "Act::AudioThread::audio_thread_init_a",
                   "An error occurred during playback."
                 );
             } catch (std::runtime_error& e) {
               Logging::write(
                 Error,
                 "Act::AudioThread::audio_thread_init_a",
                 "Error instantiating or playing Cassette: " + std::string(e.what())
               );
             }
           },
       };
     };
};

} // Act
} // Gj

#endif //AUDIOTHREAD_H
