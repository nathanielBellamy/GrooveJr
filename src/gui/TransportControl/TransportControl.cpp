//
// Created by ns on 11/17/24.
//

#include "TransportControl.h"

using namespace caf;

namespace Gj {
namespace Gui {

TransportControl::TransportControl(QWidget* parent, actor_system& sys, Audio::Mixer* mixer)
    : QToolBar(parent)
    , sys(sys)
    , currentlyPlaying(this, sys, mixer)
    {

  title.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  title.setText("GrooveJr");
  title.setFont({title.font().family(), 36});
  addWidget(&title);

  addWidget(&currentlyPlaying);

  addSeparator();
  addSeparator();

  addAction(&playTrigAction);
  addAction(&pauseTrigAction);
  addAction(&stopTrigAction);
  addAction(&rwTrigAction);
  addAction(&ffTrigAction);

  setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

  connect(&playTrigAction, &QAction::triggered, [&] {
    Logging::write(
      Info,
      "Gui::TransportControl",
      "playTrigAction"
    );
    strong_actor_ptr appStateManagerPtr = sys.registry().get(Act::ActorIds::APP_STATE_MANAGER);

    scoped_actor self{sys};
    self->anon_send(
        actor_cast<actor>(appStateManagerPtr),
        tc_trig_play_a_v
    );
  });

  connect(&pauseTrigAction, &QAction::triggered, [&] {
    Logging::write(
      Info,
      "Gui::TransportControl",
      "pauseTrigAction"
    );
    strong_actor_ptr appStateManagerPtr = sys.registry().get(Act::ActorIds::APP_STATE_MANAGER);

    const scoped_actor self{sys};
    self->anon_send(
        actor_cast<actor>(appStateManagerPtr),
        tc_trig_pause_a_v
    );
  });

  connect(&stopTrigAction, &QAction::triggered, [&] {
    Logging::write(
      Info,
      "Gui::TransportControl",
      "stopTrigAction"
    );
    strong_actor_ptr appStateManagerPtr = sys.registry().get(Act::ActorIds::APP_STATE_MANAGER);

    scoped_actor self{sys};
    self->anon_send(
        actor_cast<actor>(appStateManagerPtr),
        tc_trig_stop_a_v
    );
  });

  connect(&rwTrigAction, &QAction::triggered, [&] {
    Logging::write(
      Info,
      "Gui::TransportControl",
      "rwTrigAction"
    );
    strong_actor_ptr appStateManagerPtr = sys.registry().get(Act::ActorIds::APP_STATE_MANAGER);

    scoped_actor self{sys};
    self->anon_send(
        actor_cast<actor>(appStateManagerPtr),
        tc_trig_rw_a_v
    );
  });

  connect(&ffTrigAction, &QAction::triggered, [&] {
    Logging::write(
      Info,
      "Gui::TransportControl",
      "ffTrigAction"
    );
    strong_actor_ptr appStateManagerPtr = sys.registry().get(Act::ActorIds::APP_STATE_MANAGER);

    scoped_actor self{sys};
    self->anon_send(
        actor_cast<actor>(appStateManagerPtr),
        tc_trig_ff_a_v
    );
  });

} // constructor


int TransportControl::hydrateState(const AppStatePacket& appStatePacket) {
  const AppState appState = AppState::fromPacket(appStatePacket);
  Logging::write(
    Info,
    "Gui::TransportControl::hydrateState",
    "Received app state with playState - " + std::to_string(appState.playState)
  );

  setPlayState(appState.playState);
  return 0;
}

void TransportControl::setPlayState(PlayState newState) {
  playState = newState;
}

} // Gui
} // Gj