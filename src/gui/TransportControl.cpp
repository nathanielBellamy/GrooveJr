//
// Created by ns on 11/17/24.
//

#include "TransportControl.h"

using namespace caf;

namespace Gs {
namespace Gui {

TransportControl::TransportControl(QWidget* parent, actor_system& sys)
    : QToolBar(parent),
      sys(sys)
    {

  this->addAction(&playTrigAction);
  this->addAction(&pauseTrigAction);
  this->addAction(&stopTrigAction);
  this->addAction(&rwTrigAction);
  this->addAction(&ffTrigAction);

  this->addSeparator();
  this->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

  connect(&playTrigAction, &QAction::triggered, [&] {
    std::cout << "MainWindow : playTrigAction" << std::endl;
    strong_actor_ptr appStateManagerPtr = sys.registry().get(Gs::Act::ActorIds::APP_STATE_MANAGER);

    scoped_actor self{sys};
    self->anon_send(
        actor_cast<actor>(appStateManagerPtr),
        tc_trig_play_a_v
    );
  });

  connect(&pauseTrigAction, &QAction::triggered, [&] {
    std::cout << "MainWindow : pauseTrigAction" << std::endl;
    strong_actor_ptr appStateManagerPtr = sys.registry().get(Gs::Act::ActorIds::APP_STATE_MANAGER);

    scoped_actor self{sys};
    self->anon_send(
        actor_cast<actor>(appStateManagerPtr),
        tc_trig_pause_a_v
    );
  });

  connect(&stopTrigAction, &QAction::triggered, [&] {
    std::cout << "MainWindow : stopTrigAction" << std::endl;
    strong_actor_ptr appStateManagerPtr = sys.registry().get(Gs::Act::ActorIds::APP_STATE_MANAGER);

    scoped_actor self{sys};
    self->anon_send(
        actor_cast<actor>(appStateManagerPtr),
        tc_trig_stop_a_v
    );
  });

  connect(&rwTrigAction, &QAction::triggered, [&] {
    std::cout << "MainWindow : rwTrigAction" << std::endl;
    strong_actor_ptr appStateManagerPtr = sys.registry().get(Gs::Act::ActorIds::APP_STATE_MANAGER);

    scoped_actor self{sys};
    self->anon_send(
        actor_cast<actor>(appStateManagerPtr),
        tc_trig_rw_a_v
    );
  });

  connect(&ffTrigAction, &QAction::triggered, [&] {
    std::cout << "MainWindow : ffTrigAction" << std::endl;
    strong_actor_ptr appStateManagerPtr = sys.registry().get(Gs::Act::ActorIds::APP_STATE_MANAGER);

    scoped_actor self{sys};
    self->anon_send(
        actor_cast<actor>(appStateManagerPtr),
        tc_trig_ff_a_v
    );
  });

} // constructor


int TransportControl::hydrateState(const Gs::AppStatePacket& appStatePacket) {
    std::cout << "TransportControl : hydrateState: " << std::endl;
    AppState appState = AppState::fromPacket(appStatePacket);
    setPlayState(appState.playState);
    return 0;
}

void TransportControl::setPlayState(Gs::PlayState newState) {
  playState = newState;
  std::cout << "New State :" << newState << std::endl;
}


} // Gui
} // Gs