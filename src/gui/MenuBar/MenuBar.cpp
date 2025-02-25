//
// Created by ns on 2/23/25.
//

#include "MenuBar.h"

namespace Gj {
namespace Gui {

MenuBar::MenuBar(actor_system& actorSystem)
    : QMenuBar(nullptr)
    , actorSystem(actorSystem)
    , fileMenu(new FileMenu(actorSystem, this)) {

    addMenu(fileMenu);

}

} // Gui
} // Gj