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
  , selectSceneAction(QIcon::fromTheme(QIcon::ThemeIcon::FolderOpen), tr("&Select Scene"), this)
  , selectButtonZero(this, &selectSceneAction, 0)
  , selectButtonOne(this, &selectSceneAction, 1)
  , selectButtonTwo(this, &selectSceneAction, 2)
  , selectButtonThree(this, &selectSceneAction, 3)
  , selectButtonFour(this, &selectSceneAction, 4)
  {

  title.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  title.setText("Scene:");
  title.setFont({title.font().family(), 12});

  connectActions();
  setupGrid();
}


void Scenes::setupGrid() {
  grid.addWidget(&title, 0, 0, 1, 1);
  grid.addWidget(&selectButtonZero, 0, 1, 1, 1);
  grid.addWidget(&selectButtonOne, 0, 2, 1, 1);
  grid.addWidget(&selectButtonTwo, 0, 3, 1, 1);
  grid.addWidget(&selectButtonThree, 0, 4, 1, 1);
  grid.addWidget(&selectButtonFour, 0, 5, 1, 1);

  setLayout(&grid);
}

void Scenes::connectActions() {
  // todo
}

} // Gui
} // Gj
