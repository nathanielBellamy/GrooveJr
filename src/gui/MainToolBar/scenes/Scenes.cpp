//
// Created by ns on 6/1/25.
//

#include "Scenes.h"


namespace Gj {
namespace Gui {

Scenes::Scenes(QWidget* parent, actor_system& sys, Audio::Mixer* mixer)
  : QWidget(parent)
  , sys(sys)
  , mixer(mixer)
  , grid(this)
  , title(this)
  , sceneSaveAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSave), tr("&Save Scene"), this)
  , sceneSaveButton(this, &sceneSaveAction)
  , selectSceneAction(QIcon::fromTheme(QIcon::ThemeIcon::FolderOpen), tr("&Select Scene"), this)
  , selectButtonZero(this, &selectSceneAction, 0)
  , selectButtonOne(this, &selectSceneAction, 1)
  , selectButtonTwo(this, &selectSceneAction, 2)
  , selectButtonThree(this, &selectSceneAction, 3)
  , selectButtonFour(this, &selectSceneAction, 4)
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
  grid.addWidget(&selectButtonZero, 1, 0, 1, 1);
  grid.addWidget(&selectButtonOne, 1, 1, 1, 1);
  grid.addWidget(&selectButtonTwo, 1, 2, 1, 1);
  grid.addWidget(&selectButtonThree, 1, 3, 1, 1);
  grid.addWidget(&selectButtonFour, 1, 4, 1, 1);

  setLayout(&grid);
}

void Scenes::connectActions() {
  const auto selectSceneConnection = connect(&selectSceneAction, &QAction::triggered, [&] {
    const int sceneIndex = selectSceneAction.data().toInt();

    mixer->loadSceneByIndex(sceneIndex);
  });
}

} // Gui
} // Gj
