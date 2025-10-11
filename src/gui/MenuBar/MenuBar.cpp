//
// Created by ns on 2/23/25.
//

#include "MenuBar.h"

namespace Gj {
namespace Gui {

MenuBar::MenuBar(actor_system& actorSystem, QWidget* parent)
    : QMenuBar(parent)
    , actorSystem(actorSystem)
    , fileMenu(actorSystem, this)
    , musicMenu(actorSystem, this)
    {

    addMenu(&fileMenu);
    addMenu(&musicMenu);
}

} // Gui
} // Gj