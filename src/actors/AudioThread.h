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
#include "../audio/ThreadStatics.h"

#include "../audio/Cassette.h"

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
     Steinberg::Vst::AudioHost::App* vst3Host;

     AudioThreadState(AudioThread::pointer self, strong_actor_ptr supervisor, Steinberg::Vst::AudioHost::App* vst3Host) :
         self(self)
       , vst3Host(vst3Host)
       {
           self->link_to(supervisor);
       }

     AudioThread::behavior_type make_behavior() {
       return {
           [this](audio_thread_init_a) {
             std::cout << "AudioThread : audio_thread_init_a " << std::endl;
             Gj::Audio::Cassette cassette (
                self->system(),
                Gj::Audio::ThreadStatics::incrThreadId(),
                Gj::Audio::ThreadStatics::getFilePath(),
                Gj::Audio::ThreadStatics::getFrameId(),
                vst3Host
             );
             std::cout << "AudioThread :  cassette initiated " << std::endl;
             cassette.play();
           },
       };
     };
};

} // Act
} // Gj

#endif //AUDIOTHREAD_H
