//
// Created by ns on 2/25/25.
//

#include "MusicMenu.h"

namespace Gj {
namespace Gui {

MusicMenu::MusicMenu(actor_system& actorSystem, QWidget* parent)
    : QMenu("&Music", parent)
    , actorSystem(actorSystem)
    , addFolderToLibraryAction(new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), tr("&Add Folder To Library"), this))
    {

  addFolderToLibraryAction->setStatusTip(tr("Add folder to Music Library"));
  connect(addFolderToLibraryAction, &QAction::triggered, [&]() {
    // TODO
    std::cout << " scan folder and add to music library " << std::endl;
  });
  addAction(addFolderToLibraryAction);

}


} // Gui
} // Gj