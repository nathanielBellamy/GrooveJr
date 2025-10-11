//
// Created by ns on 2/25/25.
//

#include "MusicMenu.h"

namespace Gj {
namespace Gui {

MusicMenu::MusicMenu(actor_system& actorSystem, QWidget* parent)
    : QMenu("&Music", parent)
    , actorSystem(actorSystem)
    , addFolderToLibraryAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), tr("&Add Folder To Library"), this)
    , folderSelect(this)
    {

  addFolderToLibraryAction.setStatusTip(tr("Add Folder To Library"));
  addAction(&addFolderToLibraryAction);

  connect(&folderSelect, &QFileDialog::directoryUrlEntered, [&](const QUrl& dir) {
    folderUrl = dir;
  });

  connect(&addFolderToLibraryAction, &QAction::triggered, [&]() {
    if (folderSelect.exec() == QDialog::Accepted) {
      Logging::write(
        Info,
        "Gui::MusicMenu::addFolderToLibraryAction",
        "Selected : " + folderUrl.toDisplayString().toStdString().substr(7)
      );

      strong_actor_ptr musicLibraryManager = actorSystem.registry().get(Act::ActorIds::MUSIC_LIBRARY_MANAGER);

      scoped_actor self{actorSystem};
      self->anon_send(
          actor_cast<actor>(musicLibraryManager),
          folderUrl.toDisplayString().toStdString().substr(7),
          ml_scan_dir_a_v
      );
    }
  });
}

} // Gui
} // Gj