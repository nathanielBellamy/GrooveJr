//
// Created by ns on 10/20/24.
//

#ifndef DISPLAY_H
#define DISPLAY_H

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
#include "../gui/MainWindow.h"

using namespace caf;

namespace Gj {
namespace Act {

struct DisplayTrait {

    using signatures = type_list<
                                  result<void>(strong_actor_ptr, hydrate_display_a),
                                  result<void>(strong_actor_ptr, AppStatePacket, current_state_a)
                                >;

};

using Display = typed_actor<DisplayTrait>;

struct DisplayState {

     Display::pointer self;
     Gui::MainWindow* mainWindow;

     DisplayState(Display::pointer self, strong_actor_ptr supervisor, Gui::MainWindow* mainWindow) :
         self(self)
       , mainWindow(mainWindow)
       {
           self->link_to(supervisor);
           self->system().registry().put(DISPLAY, actor_cast<strong_actor_ptr>(self));
       }

     Display::behavior_type make_behavior() {
       return {
           [this](strong_actor_ptr replyToPtr, hydrate_display_a) {
             std::cout << "Display : hydrate_display_a " << std::endl;
             strong_actor_ptr appStateManager = self->system().registry().get(APP_STATE_MANAGER);
             self->anon_send(
                 actor_cast<actor>(appStateManager),
                 actor_cast<strong_actor_ptr>(self),
                 read_state_a_v
             );
           },
           [this](strong_actor_ptr replyToPtr, Gj::AppStatePacket appStatePacket, current_state_a) {
             std::cout << "Display : current_state_a " << std::endl;
             if ( mainWindow->hydrateState(appStatePacket) )
               std::cout << "Display : Error : unable to hydrate state " << std::endl;
           },
       };
     };
};

} // Act
} // Gj

#endif //DISPLAY_H
