//
// Created by ns on 2/23/25.
//

#include "FileMenu.h"

namespace Gj {
namespace Gui {

FileMenu::FileMenu(actor_system& actorSystem, QWidget* parent)
    : QMenu("File", parent)
    , actorSystem(actorSystem)
    {



}

} // Gui
} // Gj