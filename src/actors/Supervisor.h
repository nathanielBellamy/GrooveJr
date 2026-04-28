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

#include "QApplication"
#include "QIcon"

#include "../Logging.h"
#include "./ActorIds.h"
#include "../messaging/atoms.h"
#include "./AppStateManager.h"
#include "./Display.h"
#include "./Playback.h"
#include "./MusicLibraryManager.h"

#include "../gui/MainWindow.h"
#include "../audio/mixer/Core.h"
#include "../audio/engine/AudioCore.h"

using namespace caf;

namespace Gj {
namespace Act {
struct SupervisorTrait {
  using signatures = type_list<result<void>(strong_actor_ptr, supervisor_status_a)>;
};

using Supervisor = typed_actor<SupervisorTrait>;

// TODO: actor formatting
struct SupervisorState {
  strong_actor_ptr playbackActorPtr;
  strong_actor_ptr appStateManagerPtr;
  strong_actor_ptr musicLibraryManagerPtr;
  strong_actor_ptr displayPtr;
  Supervisor::pointer self;

  SupervisorState(
    Supervisor::pointer self,
    State::Core* stateCore,
    Audio::Mixer::Core* mixer,
    Audio::AudioCore* audioCore,
    void (*shutdown_handler)(int)
  )
  : self(self) {
    self->system().registry().put(SUPERVISOR, actor_cast<strong_actor_ptr>(self));
    auto playback = self->system().spawn(
      actor_from_state<PlaybackState>,
      actor_cast<strong_actor_ptr>(self),
      stateCore,
      mixer,
      audioCore
    );
    playbackActorPtr = actor_cast<strong_actor_ptr>(playback);

    auto appStateManager = self->system().spawn(
      actor_from_state<AppStateManagerState>,
      actor_cast<strong_actor_ptr>(self),
      stateCore,
      mixer,
      mixer->dao,
      audioCore
    );
    appStateManagerPtr = actor_cast<strong_actor_ptr>(appStateManager);

    QApplication* qtApp = qobject_cast<QApplication*>(QApplication::instance());
    bool createdApp = false;
    if (!qtApp) {
      constexpr int arg = 0;
      static int argc = arg;
      static char* argv[1] = {nullptr};
      qtApp = new QApplication{argc, argv};
      createdApp = true;
    }
    qtApp->setWindowIcon(QIcon(":/icons/GrooveJr.png"));
    auto display = self->system().spawn(
      actor_from_state<DisplayState>,
      actor_cast<strong_actor_ptr>(self),
      mixer,
      stateCore,
      shutdown_handler
    );
    displayPtr = actor_cast<strong_actor_ptr>(display);

    auto musicLibraryManager = self->system().spawn(
      actor_from_state<MusicLibraryManagerState>,
      actor_cast<strong_actor_ptr>(self),
      mixer,
      stateCore
    );
    musicLibraryManagerPtr = actor_cast<strong_actor_ptr>(musicLibraryManager);

    self->anon_send(
      actor_cast<actor>(appStateManagerPtr),
      hydrate_display_a_v
    );

    if (createdApp) {
      qtApp->exec();
    }
  }

  Supervisor::behavior_type make_behavior() {
    return {
      [this](strong_actor_ptr replyToPtr, supervisor_status_a) {
        Logging::write(
          Info,
          "Act::Supervisor::supervisor_status_a",
          "Supervisor running"
        );
      },
    };
  };
};
} // Act
} // Gj

#endif //SUPERVISOR_H
