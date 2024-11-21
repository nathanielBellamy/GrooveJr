//
// Created by ns on 10/20/24.
//

#ifndef SUPERVISOR_H
#define SUPERVISOR_H

#include "caf/actor_ostream.hpp"
#include "caf/actor_registry.hpp"
#include "caf/actor_system.hpp"
#include "caf/caf_main.hpp"
#include "caf/event_based_actor.hpp"

#include "./ActorIds.h"
#include "../messaging/atoms.h"
#include "./AppStateManager.h"
#include "./Playback.h"
#include "./Display.h"

#include "../gui/MainWindow.h"

using namespace caf;

namespace Gs {
namespace Act {

struct SupervisorTrait {

    using signatures = type_list<result<void>(strong_actor_ptr, supervisor_status_a)>;

};

using Supervisor = typed_actor<SupervisorTrait>;

struct SupervisorState {
     bool running;
     strong_actor_ptr playbackActorPtr;
     strong_actor_ptr appStateManagerPtr;
     strong_actor_ptr displayPtr;
     Gs::Gui::MainWindow* mainWindowPtr;

     Supervisor::pointer self;

     SupervisorState(Supervisor::pointer self, Gs::Gui::MainWindow* mainWindowPtr) :
         self(self)
       , mainWindowPtr(mainWindowPtr)
       , running(false)
         {
           self->system().registry().put(ActorIds::SUPERVISOR, actor_cast<strong_actor_ptr>(self));
//           auto gs_app_state_manager = sys.spawn(actor_from_state<gs_app_state_manager_state>);
//           auto gs_controller = sys.spawn(actor_from_state<gs_controller_state
           auto playback = self->system().spawn(actor_from_state<PlaybackState>, actor_cast<strong_actor_ptr>(self));
           playbackActorPtr = actor_cast<strong_actor_ptr>(playback);

           auto appStateManager = self->system().spawn(
               actor_from_state<AppStateManagerState>,
               actor_cast<strong_actor_ptr>(self)
           );
           appStateManagerPtr = actor_cast<strong_actor_ptr>(appStateManager);

           auto display = self->system().spawn(
               actor_from_state<DisplayState>,
               actor_cast<strong_actor_ptr>(self),
               mainWindowPtr
           );
           displayPtr = actor_cast<strong_actor_ptr>(display);

           running = true;
         }

     Supervisor::behavior_type make_behavior() {
       return {
           [this](strong_actor_ptr replyToPtr, supervisor_status_a) {
             std::cout << "Supervisor running : " << this->running << std::endl;
//             this->self->anon_send(
//                 actor_cast<strong_actor_ptr>(replyToPtr),
//                 this->running
//             );
           },
       };
     };
};

} // Act
} // Gs

#endif //SUPERVISOR_H
