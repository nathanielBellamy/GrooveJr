//
// Created by ns on 11/17/24.
//

#include "TransportControl.h"

using namespace caf;

namespace Gj {
namespace Gui {

TransportControl::TransportControl(QWidget* parent, actor_system& sys)
    : QToolBar(parent),
      sys(sys)
    {

  addAction(&playTrigAction);
  addAction(&pauseTrigAction);
  addAction(&stopTrigAction);
  addAction(&rwTrigAction);
  addAction(&ffTrigAction);

  addSeparator();
  setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

  connect(&playTrigAction, &QAction::triggered, [&] {
    std::cout << "TransportControl : playTrigAction" << std::endl;
    strong_actor_ptr appStateManagerPtr = sys.registry().get(Gj::Act::ActorIds::APP_STATE_MANAGER);

    scoped_actor self{sys};
    self->anon_send(
        actor_cast<actor>(appStateManagerPtr),
        tc_trig_play_a_v
    );
  });

  connect(&pauseTrigAction, &QAction::triggered, [&] {
    std::cout << "TransportControl : pauseTrigAction" << std::endl;
    strong_actor_ptr appStateManagerPtr = sys.registry().get(Gj::Act::ActorIds::APP_STATE_MANAGER);

    scoped_actor self{sys};
    self->anon_send(
        actor_cast<actor>(appStateManagerPtr),
        tc_trig_pause_a_v
    );
  });

  connect(&stopTrigAction, &QAction::triggered, [&] {
    std::cout << "TransportControl : stopTrigAction" << std::endl;
    strong_actor_ptr appStateManagerPtr = sys.registry().get(Gj::Act::ActorIds::APP_STATE_MANAGER);

    scoped_actor self{sys};
    self->anon_send(
        actor_cast<actor>(appStateManagerPtr),
        tc_trig_stop_a_v
    );
  });

  connect(&rwTrigAction, &QAction::triggered, [&] {
    std::cout << "TransportControl : rwTrigAction" << std::endl;
    strong_actor_ptr appStateManagerPtr = sys.registry().get(Gj::Act::ActorIds::APP_STATE_MANAGER);

    scoped_actor self{sys};
    self->anon_send(
        actor_cast<actor>(appStateManagerPtr),
        tc_trig_rw_a_v
    );
  });

  connect(&ffTrigAction, &QAction::triggered, [&] {
    std::cout << "TransportControl : ffTrigAction" << std::endl;
    strong_actor_ptr appStateManagerPtr = sys.registry().get(Gj::Act::ActorIds::APP_STATE_MANAGER);

    scoped_actor self{sys};
    self->anon_send(
        actor_cast<actor>(appStateManagerPtr),
        tc_trig_ff_a_v
    );
  });

} // constructor


int TransportControl::hydrateState(const Gj::AppStatePacket& appStatePacket) {
    std::cout << "TransportControl : hydrateState: " << std::endl;
    AppState appState = AppState::fromPacket(appStatePacket);
    setPlayState(appState.playState);
    return 0;
}

void TransportControl::setPlayState(Gj::PlayState newState) {
  playState = newState;
  std::cout << "New State :" << newState << std::endl;
}


} // Gui
} // Gj