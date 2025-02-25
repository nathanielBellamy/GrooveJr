//
// Created by ns on 2/23/25.
//

#ifndef MENUBAR_H
#define MENUBAR_H

#include <iostream>

#include "caf/actor_from_state.hpp"
#include "caf/actor_ostream.hpp"
#include "caf/actor_registry.hpp"
#include "caf/actor_system.hpp"
#include "caf/caf_main.hpp"
#include "caf/event_based_actor.hpp"
#include "caf/scoped_actor.hpp"

#include <QMenuBar>

#include "FileMenu.h"

namespace Gj {
namespace Gui {

using namespace caf;

class MenuBar final : QMenuBar {
  Q_OBJECT

private:
  actor_system& actorSystem;
  FileMenu* fileMenu;
  QAction fileMenuAction;

public:
  MenuBar(actor_system&);

};

} // Gui
} // Gj

#endif //MENUBAR_H
