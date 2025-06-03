//
// Created by ns on 6/1/25.
//

#include "Scenes.h"

namespace Gj {
namespace Gui {

Scenes::Scenes(QWidget* parent, actor_system& sys, Audio::Mixer* mixer, QAction* sceneLoadAction)
  : QWidget(parent)
  , sys(sys)
  , mixer(mixer)
  , grid(this)
  , title(this)
  , sceneSaveAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSave), tr("&Save Scene"), this)
  , sceneSaveButton(this, &sceneSaveAction)
  , selectButtonZero(this, sceneLoadAction, 0)
  , selectButtonOne(this, sceneLoadAction, 1)
  , selectButtonTwo(this, sceneLoadAction, 2)
  , selectButtonThree(this, sceneLoadAction, 3)
  , selectButtonFour(this, sceneLoadAction, 4)
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
  const auto saveSceneConnection = connect(&sceneSaveAction, &QAction::triggered, [&] {
    if (mixer->saveScene() != 0)
      Logging::write(
        Error,
        "Gui::Scenes::connectActions",
        "Unable to save scene"
      );
  });
}

} // Gui
} // Gj
