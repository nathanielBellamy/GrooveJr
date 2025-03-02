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
#include "./Mixer/MixerWindow.h"
#include "./TransportControl.h"

#include <QMainWindow>
#include <QGridLayout>

using namespace caf;

namespace Gj {
namespace Gui {

class MainWindow final : public QMainWindow {
  public:
    explicit MainWindow(actor_system& actorSystem);
    int hydrateState(const AppStatePacket& appStatePacket);

  private:
    actor_system& actorSystem;
    QWidget container;
    MenuBar* menuBar;
    TransportControl transportControl;
    QGridLayout grid;
    MixerWindow mixerWindow;

    void initGrid();
};

} // Gui
} // Gj



#endif //MAINWINDOW_H
