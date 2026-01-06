//
// Created by ns on 6/1/25.
//

#include "Scenes.h"

namespace Gj {
namespace Gui {
Scenes::Scenes(
  QWidget* parent,
  actor_system& sys,
  State::Core* stateCore,
  Audio::Mixer::Core* mixer,
  SqlWorkerPool* sqlWorkerPool,
  QAction* sceneLoadAction)
: QWidget(parent)
  , sys(sys)
  , mixer(mixer)
  , grid(new QGridLayout(this))
  , title(new QLabel(this))
  , sceneNewAction(new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew), tr("&New"), this))
  , sceneNewButton(new SceneButton(this, sceneNewAction, QString("New")))
  , sceneSaveAction(new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSave), tr("&Save"), this))
  , sceneSaveButton(new SceneButton(this, sceneSaveAction, QString("Save")))
  , sceneLoadAction(sceneLoadAction) {
  tableView = new ScenesTableView(
    this,
    sys,
    mixer->dao,
    stateCore,
    sqlWorkerPool,
    sceneLoadAction
  );

  title->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  title->setText("Scenes");
  title->setFont({title->font().family(), 12});

  connectActions();
  setupGrid();
  setStyle();

  Logging::write(
    Info,
    "Gui::Scenes::Scenes()",
    "Constructed Scenes"
  );
}

Scenes::~Scenes() {
  delete sceneSaveButton;
  delete sceneSaveAction;
  delete sceneNewButton;
  delete sceneNewAction;
  delete title;
  delete grid;
}


Result Scenes::setStyle() {
  setMinimumWidth(300);
  return OK;
}

void Scenes::setupGrid() {
  grid->setRowStretch(0, 1);
  grid->setRowStretch(1, 10);
  grid->addWidget(title, 0, 0, 1, -1);
  grid->addWidget(sceneNewButton, 0, 2, 1, 1);
  grid->addWidget(sceneSaveButton, 0, 3, 1, 1);
  grid->addWidget(tableView, 1, 0, -1, -1);

  setLayout(grid);
}

void Scenes::connectActions() {
  const auto newSceneConnection = connect(sceneNewAction, &QAction::triggered, [&] {
    const auto sceneId = mixer->newScene();
    Logging::write(
      Info,
      "Gui::Scenes::sceneNewAction",
      "New scene id: " + std::to_string(sceneId)
    );
    tableView->refresh(true);
  });

  const auto saveSceneConnection = connect(sceneSaveAction, &QAction::triggered, [&] {
    const auto sceneDbId = mixer->saveScene();
    Logging::write(
      Info,
      "Gui::Scenes::sceneSaveAction",
      "Saved scene id: " + std::to_string(sceneDbId)
    );
    tableView->refresh(true);
  });
}

Result Scenes::hydrateState(const State::Packet& statePacket) {
  return OK;
}
} // Gui
} // Gj
