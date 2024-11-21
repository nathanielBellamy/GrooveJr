// TODO:
// - jk, we want a Display Actor
// - we do not want Actors to have to know about Qt classes
// - so when any actor needs to update the display
//   they will ping Display to hydrate state
// - Display will be instantiated with a pointer to MainWindow
// - Display will cally MainWindow::hydrateState
// - all changes flow down from there redux-style

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

namespace Gs {
namespace Act {

struct DisplayTrait {

    using signatures = type_list<
                                  result<void>(strong_actor_ptr, hydrate_display_a),
                                  result<void>(strong_actor_ptr, Gs::AppStatePacket, current_state_a)
                                >;

};

using Display = typed_actor<DisplayTrait>;

struct DisplayState {
     Gs::Gui::MainWindow* mainWindow;

     Display::pointer self;

     DisplayState(Display::pointer self, strong_actor_ptr supervisor, Gs::Gui::MainWindow* mainWindow) :
         self(self)
       , mainWindow(mainWindow)
       {
           self->link_to(supervisor);
           self->system().registry().put(ActorIds::DISPLAY, actor_cast<strong_actor_ptr>(self));
       }

     Display::behavior_type make_behavior() {
       return {
           [this](strong_actor_ptr replyToPtr, hydrate_display_a) {
             std::cout << "Display : hydrate_display_a " << std::endl;
             strong_actor_ptr appStateManager = self->system().registry().get(ActorIds::APP_STATE_MANAGER);
             this->self->anon_send(
                 actor_cast<actor>(appStateManager),
                 actor_cast<strong_actor_ptr>(self),
                 read_state_a_v
             );
           },
           [this](strong_actor_ptr replyToPtr, Gs::AppStatePacket appStatePacket, current_state_a) {
             std::cout << "Display : current_state_a " << std::endl;
             if ( mainWindow->hydrateState(appStatePacket) )
               std::cout << "Display : Error : unable to hydrate state " << std::endl;
           },
       };
     };
};

} // Act
} // Gs

#endif //DISPLAY_H
