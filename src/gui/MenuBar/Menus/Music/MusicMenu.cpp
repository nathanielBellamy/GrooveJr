//
// Created by ns on 2/25/25.
//

#include "MusicMenu.h"

namespace Gj {
namespace Gui {

MusicMenu::~MusicMenu() {
  delete addFolderToLibraryAction;
  delete folderSelect;
  delete this;
}


MusicMenu::MusicMenu(actor_system& actorSystem, QWidget* parent)
    : QMenu("&Music", parent)
    , actorSystem(actorSystem)
    , addFolderToLibraryAction(new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), tr("&Add Folder To Library"), this))
    , folderSelect(new FolderSelect(this))
    {

  addFolderToLibraryAction->setStatusTip(tr("Add folder to Music Library"));
  addAction(addFolderToLibraryAction);

  connect(folderSelect, &QFileDialog::directoryUrlEntered, [&](const QUrl& dir) {
    folderUrl = dir;
  });

  connect(addFolderToLibraryAction, &QAction::triggered, [&]() {
    if (folderSelect->exec() == QDialog::Accepted) {
      std::cout << "folder selected: " << folderUrl.toDisplayString().toStdString() << std::endl;
    }
  });
}


} // Gui
} // Gj