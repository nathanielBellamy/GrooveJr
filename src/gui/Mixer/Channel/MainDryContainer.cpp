//
// Created by ns on 3/8/25.
//

#include "MainDryContainer.h"

namespace Gj {
namespace Gui {

MainDryContainer::MainDryContainer(QWidget* parent, actor_system& actorSystem)
  : QWidget(parent)
  , actorSystem(actorSystem)
  , grid(this)
  , dryChannel(this, actorSystem)
  , mainChannel(this, actorSystem, 0)
  {

  setupGrid();
  setStyle();
}

void MainDryContainer::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setStyleSheet("background-color: red; border: 2px solid white; border-radius: 5px;");
}

void MainDryContainer::setupGrid() {

  grid.addWidget(&mainChannel, 0, 0, -1, 1);
  grid.addWidget(&dryChannel, 0, 1, -1, 1);
}

} // Gui
} // Gj