//
// Created by ns on 11/4/24.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <format>

#include "caf/actor_from_state.hpp"
#include "caf/actor_ostream.hpp"
#include "caf/actor_registry.hpp"
#include "caf/actor_system.hpp"
#include "caf/caf_main.hpp"
#include "caf/event_based_actor.hpp"
#include "caf/scoped_actor.hpp"

#include "../actors/ActorIds.h"
#include "../messaging/atoms.h"
#include "../messaging/EnvelopeQtPtr.h"

#include "./MenuBar/MenuBar.h"
#include "./TransportControl.h"

#include <QFrame>
#include <QMainWindow>
#include <QLabel>
#include <QScreen>
#include <QToolbar>
#include <QToolButton>
#include <QAction>
#include <QStyle>

using namespace caf;

namespace Gj {
namespace Gui {

class MainWindow final : public QMainWindow {
  public:
    explicit MainWindow(actor_system& actorSystem);
    int hydrateState(const Gj::AppStatePacket& appStatePacket);

  private:
    QFrame frame;
    QLabel label {&frame};
    actor_system& actorSystem;
    MenuBar* menuBar;
    TransportControl transportControl;
};

} // Gui
} // Gj



#endif //MAINWINDOW_H
