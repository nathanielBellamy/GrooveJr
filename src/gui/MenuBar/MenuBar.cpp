//
// Created by ns on 2/23/25.
//

#include "MenuBar.h"

namespace Gj {
namespace Gui {

MenuBar::MenuBar(actor_system& actorSystem, QWidget* parent)
    : QMenuBar(parent)
    , actorSystem(actorSystem)
    , fileMenu(new FileMenu(actorSystem, this))
    , musicMenu(new MusicMenu(actorSystem, this))
    , effectsMenu(new EffectsMenu(actorSystem, this))
    {

    addMenu(fileMenu);
    addMenu(musicMenu);
    addMenu(effectsMenu);

}

} // Gui
} // Gj