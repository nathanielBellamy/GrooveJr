//
// Created by ns on 10/20/24.
//

#ifndef DISPLAY_H
#define DISPLAY_H

#include "caf/actor_registry.hpp"
#include "caf/actor_system.hpp"
#include "caf/event_based_actor.hpp"

#include "QApplication"

#include "../enums/Result.h"
#include "../Logging.h"
#include "./ActorIds.h"
#include "../messaging/atoms.h"
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
  QApplication* qtApp;
  Gui::MainWindow* mainWindow;
  strong_actor_ptr guiThreadPtr;

  DisplayState(
    Display::pointer self,
    strong_actor_ptr supervisor,
    Audio::Mixer* mixer,
    AppState* gAppState,
    void (*shutdown_handler) (int)
    )
    : self(self)
    {
      self->link_to(supervisor);
      self->system().registry().put(DISPLAY, actor_cast<strong_actor_ptr>(self));

      mainWindow = new Gui::MainWindow { self->system(), mixer, gAppState, shutdown_handler };
      mainWindow->show();
      mainWindow->setChannels();
      mainWindow->setEffects();
    }

  ~DisplayState() {
    delete mainWindow;
    Logging::write(
      Info,
      "Act::Display::~Display()",
      "Destroyed Display"
     );
  }

  Display::behavior_type make_behavior() {
    return {
      [this](strong_actor_ptr replyToPtr, hydrate_display_a) {
        Logging::write(
          Info,
          "Act::Display::hydrate_display_a",
          "Received hydrate_display_a"
        );
        strong_actor_ptr appStateManager = self->system().registry().get(APP_STATE_MANAGER);
        self->anon_send(
          actor_cast<actor>(appStateManager),
          actor_cast<strong_actor_ptr>(self),
          read_state_a_v
        );
      },
      [this](strong_actor_ptr replyToPtr, AppStatePacket appStatePacket, current_state_a) {
        Logging::write(
          Info,
          "Act::Display::current_state_a",
          "Received current state, will hydrate display."
        );
        if (mainWindow->hydrateState(appStatePacket) == ERROR)
          Logging::write(
            Error,
            "Act::Display::current_state_a",
            "Unable to hydrate state to main window"
          );
      },
    };
  };
};

} // Act
} // Gj

#endif //DISPLAY_H
