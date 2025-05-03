//
// Created by ns on 3/8/25.
//

#include "MainChannelContainer.h"

namespace Gj {
namespace Gui {

MainChannelContainer::MainChannelContainer(QWidget* parent, actor_system& actorSystem, Audio::Mixer* mixer)
  : QWidget(parent)
  , actorSystem(actorSystem)
  , mixer(mixer)
  , grid(this)
  , mainChannel(this, actorSystem, mixer, 0, nullptr)
  {
  setupGrid();
  setStyle();
}

void MainChannelContainer::hydrateState(const AppStatePacket &appState) {
  mainChannel.hydrateState(appState);
}

void MainChannelContainer::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setStyleSheet("background-color: red; border: 2px solid white; border-radius: 5px;");
}

void MainChannelContainer::setupGrid() {
  grid.addWidget(&mainChannel, 0, 0, -1, -1);
}

} // Gui
} // Gj