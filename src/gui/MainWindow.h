//
// Created by ns on 11/4/24.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>

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
#include "../enums/PlayState.h"

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

namespace Gs {
namespace Gui {

class MainWindow : public QMainWindow {
  public:
    MainWindow(actor_system& sys);
    int hydrateState(const Gs::AppStatePacket& appStatePacket);

  private:
    QFrame frame;
    QLabel label {&frame};
    TransportControl transportControl;
    actor_system& sys;
    Gs::PlayState playState;
};

} // Gui
} // Gs



#endif //MAINWINDOW_H
