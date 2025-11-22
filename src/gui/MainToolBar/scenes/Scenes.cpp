//
// Created by ns on 6/1/25.
//

#include "Scenes.h"

namespace Gj {
namespace Gui {

Scenes::Scenes(
  QWidget* parent,
  actor_system& sys,
  AppState* gAppState,
  Audio::Mixer* mixer,
  SqlWorkerPool* sqlWorkerPool,
  QAction* sceneLoadAction)
  : QWidget(parent)
  , sys(sys)
  , mixer(mixer)
  , grid(this)
  , title(this)
  , sceneNewAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew), tr("&New"), this)
  , sceneNewButton(this, &sceneNewAction, QString("New"))
  , sceneSaveAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSave), tr("&Save"), this)
  , sceneSaveButton(this, &sceneSaveAction, QString("Save"))
  , sceneLoadAction(sceneLoadAction)
  {

  tableView = new ScenesTableView(
    this,
    sys,
    mixer->dao,
    gAppState,
    sqlWorkerPool,
    sceneLoadAction
  );

  title.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  title.setText("Scenes");
  title.setFont({title.font().family(), 12});

  connectActions();
  setupGrid();
  setStyle();
}

Result Scenes::setStyle() {
  setMinimumWidth(300);
  return OK;
}

void Scenes::setupGrid() {
  grid.addWidget(&title, 0, 0, 1, -1);
  grid.addWidget(&sceneNewButton, 0, 2, 1, 1);
  grid.addWidget(&sceneSaveButton, 0, 3, 1, 1);
  grid.addWidget(tableView, 1, 0, -1, -1);

  setLayout(&grid);
}

void Scenes::connectActions() {
  const auto newSceneConnection = connect(&sceneNewAction, &QAction::triggered, [&] {
    const auto sceneId = mixer->newScene();
    Logging::write(
      Info,
      "Gui::Scenes::sceneNewAction",
      "New scene id: " + std::to_string(sceneId)
    );
    tableView->refresh(true);
  });

  const auto saveSceneConnection = connect(&sceneSaveAction, &QAction::triggered, [&] {
    const auto sceneDbId = mixer->saveScene();
    Logging::write(
      Info,
      "Gui::Scenes::sceneSaveAction",
      "Saved scene id: " + std::to_string(sceneDbId)
    );
    tableView->refresh(true);
  });
}

Result Scenes::hydrateState(const AppStatePacket& appStatePacket) {
  return OK;
}

} // Gui
} // Gj
