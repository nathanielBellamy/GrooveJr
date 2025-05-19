//
// Created by ns on 11/17/24.
//

#include "TransportControl.h"

using namespace caf;

namespace Gj {
namespace Gui {

TransportControl::TransportControl(QWidget* parent, actor_system& sys, Audio::Mixer* mixer)
  : QWidget(parent)
  , sys(sys)
  , grid(this)
  , progressBar(this, mixer, 0)
  , playButton(this, &playTrigAction, style()->standardIcon(QStyle::StandardPixmap::SP_MediaPlay))
  , pauseButton(this, &pauseTrigAction, style()->standardIcon(QStyle::StandardPixmap::SP_MediaPause))
  , stopButton(this, &stopTrigAction, style()->standardIcon(QStyle::StandardPixmap::SP_MediaStop))
  , rwButton(this, &rwTrigAction, style()->standardIcon(QStyle::StandardPixmap::SP_MediaSkipBackward))
  , ffButton(this, &ffTrigAction, style()->standardIcon(QStyle::StandardPixmap::SP_MediaSkipForward))
  {

  connectActions();
  setupGrid();
}


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

void TransportControl::connectActions() {
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
}

void TransportControl::setupGrid() {

  grid.addWidget(&playButton, 0, 0, 1, 1);
  grid.addWidget(&pauseButton, 0, 1, 1, 1);
  grid.addWidget(&stopButton, 0, 2, 1, 1);
  grid.addWidget(&rwButton, 0, 3, 1, 1);
  grid.addWidget(&ffButton, 0, 4, 1, 1);
  grid.addWidget(&progressBar, 1, 0, 1, -1);

  setLayout(&grid);
}

} // Gui
} // Gj