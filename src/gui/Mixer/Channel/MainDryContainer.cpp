//
// Created by ns on 3/8/25.
//

#include "MainDryContainer.h"

namespace Gj {
namespace Gui {

MainDryContainer::MainDryContainer(QWidget* parent, actor_system& actorSystem, Audio::Mixer* mixer)
  : QWidget(parent)
  , actorSystem(actorSystem)
  , mixer(mixer)
  , grid(this)
  , dryChannel(this, actorSystem)
  , mainChannel(this, actorSystem, mixer, 0)
  {

  setupGrid();
  setStyle();
}

void MainDryContainer::hydrateState(const AppStatePacket &appState) {
  // todo
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