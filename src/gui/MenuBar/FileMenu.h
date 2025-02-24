//
// Created by ns on 2/23/25.
//

#ifndef FILEMENU_H
#define FILEMENU_H

#include "caf/actor_from_state.hpp"
#include "caf/actor_ostream.hpp"
#include "caf/actor_registry.hpp"
#include "caf/actor_system.hpp"
#include "caf/caf_main.hpp"
#include "caf/event_based_actor.hpp"
#include "caf/scoped_actor.hpp"

#include <QMenu>

namespace Gj {
namespace Gui {

using namespace caf;

class FileMenu final : QMenu {

  private:
    actor_system& actorSystem;

  public:
    FileMenu(actor_system& actorSystem, QWidget* parent);

};

} // Gui
} // Gj



#endif //FILEMENU_H
