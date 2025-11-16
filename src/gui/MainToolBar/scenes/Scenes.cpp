//
// Created by ns on 6/1/25.
//

#include "Scenes.h"

namespace Gj {
namespace Gui {

Scenes::Scenes(QWidget* parent, actor_system& sys, AppState* gAppState, Audio::Mixer* mixer, QAction* sceneLoadAction)
  : QWidget(parent)
  , sys(sys)
  , mixer(mixer)
  , grid(this)
  , title(this)
  , tableView(this, sys, mixer->dao, gAppState, nullptr) // TODO: lift and pass workerPool
  , sceneSaveAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSave), tr("&Save Scene"), this)
  , sceneSaveButton(this, &sceneSaveAction)
  {

  title.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  title.setText("Scenes");
  title.setFont({title.font().family(), 12});

  connectActions();
  setupGrid();
}


void Scenes::setupGrid() {
  grid.addWidget(&title, 0, 0, 1, -1);
  grid.addWidget(&sceneSaveButton, 0, 3, 1, 2);
  grid.addWidget(&tableView, 1, 0, -1, -1);

  setLayout(&grid);
}

void Scenes::connectActions() {
  const auto saveSceneConnection = connect(&sceneSaveAction, &QAction::triggered, [&] {
    const auto sceneId = mixer->saveScene();
    Logging::write(
      Info,
      "Gui::Scenes::sceneSaveAction",
      "Saved scene id: " + std::to_string(sceneId)
    );
  });
}

Result Scenes::hydrateState(const AppStatePacket& appStatePacket) {
  return OK;
}

} // Gui
} // Gj
