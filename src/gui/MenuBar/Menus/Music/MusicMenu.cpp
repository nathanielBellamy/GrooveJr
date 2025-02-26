//
// Created by ns on 2/25/25.
//

#include "MusicMenu.h"

namespace Gj {
namespace Gui {

MusicMenu::~MusicMenu() {
  delete addFolderToLibraryAction;
  delete folderSelect;
}


MusicMenu::MusicMenu(actor_system& actorSystem, QWidget* parent)
    : QMenu("&Music", parent)
    , actorSystem(actorSystem)
    , addFolderToLibraryAction(new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), tr("&Add Folder To Library"), this))
    , folderUrl(QUrl())
    {

  folderSelect = new FolderSelect(this);
  connect(folderSelect, &QFileDialog::directoryUrlEntered, [&](const QUrl& dir) {
    folderUrl = dir;
  });


  addFolderToLibraryAction->setStatusTip(tr("Add folder to Music Library"));
  connect(addFolderToLibraryAction, &QAction::triggered, [&]() {
    if (folderSelect->exec() == QDialog::Accepted) {
      auto url = QUrl();
      folderSelect->urlSelected(url);

      std::cout << "folder selected: " << folderUrl.toDisplayString().toStdString() << std::endl;
    }
  });

  addAction(addFolderToLibraryAction);
}


} // Gui
} // Gj