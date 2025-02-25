//
// Created by ns on 2/23/25.
//

#include "FileMenu.h"

#include <iostream>

namespace Gj {
namespace Gui {

FileMenu::FileMenu(actor_system& actorSystem, QWidget* parent)
    : QMenu("File", parent)
    , actorSystem(actorSystem)
    , openAction(new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen),
                   tr("&Open"), this))
    {

  openAction->setShortcuts(QKeySequence::Open);
  openAction->setStatusTip(tr("Open an existing file"));
  connect(openAction, &QAction::triggered, [&]() {
    std::cout << "open file" << std::endl;
  });

}

} // Gui
} // Gj