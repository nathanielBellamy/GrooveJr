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

#include "./ActorIds.h"
#include "../messaging/atoms.h"
#include "./AppStateManager.h"
#include "./Playback.h"
#include "../AppState.h"

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

     AudioThread::pointer self;

     AudioThreadState(AudioThread::pointer self, strong_actor_ptr supervisor) :
         self(self)
       {
           self->link_to(supervisor);
           self->system().registry().put(ActorIds::AUDIO_THREAD, actor_cast<strong_actor_ptr>(self));
       }

     AudioThread::behavior_type make_behavior() {
       return {
           [this](audio_thread_init_a) {
             std::cout << "AudioThread : audio_thread_init_a " << std::endl;
           },
       };
     };
};

} // Act
} // Gj

#endif //AUDIOTHREAD_H
