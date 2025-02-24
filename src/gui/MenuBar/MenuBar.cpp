//
// Created by ns on 2/23/25.
//

#include "MenuBar.h"

namespace Gj {
namespace Gui {

MenuBar::MenuBar(actor_system& actorSystem)
    : QMenuBar(nullptr)
    , actorSystem(actorSystem)
    {

    QMenu* fileMenu = new QMenu("File", this);
    addMenu(fileMenu);

}

} // Gui
} // Gj