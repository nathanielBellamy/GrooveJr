//
// Created by ns on 3/8/25.
//

#include "MainChannelContainer.h"

namespace Gj {
namespace Gui {

MainChannelContainer::MainChannelContainer(
  QWidget* parent,
  actor_system& actorSystem,
  Audio::Mixer* mixer,
  QAction* muteChannelAction,
  QAction* muteLChannelAction,
  QAction* muteRChannelAction,
  QAction* soloChannelAction,
  QAction* soloLChannelAction,
  QAction* soloRChannelAction
  )
  : QWidget(parent)
  , actorSystem(actorSystem)
  , mixer(mixer)
  , grid(this)
  , mainChannel(
    this, actorSystem, mixer, 0, nullptr,
    muteChannelAction, muteLChannelAction, muteRChannelAction,
    soloChannelAction, soloLChannelAction, soloRChannelAction
  ) {
  setupGrid();
  setStyle();
}

void MainChannelContainer::hydrateState(const AppStatePacket &appState) {
  mainChannel.hydrateState(appState, 0);
}

void MainChannelContainer::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  std::string styleString = "border-radius: 5px; ";
  styleString += "background-color: " + Color::toHex(GjC::LIGHT_200) + "; ";
  setStyleSheet(styleString.data());
}

void MainChannelContainer::setupGrid() {
  grid.addWidget(&mainChannel, 0, 0, -1, -1);
}

int MainChannelContainer::addEffect(const std::string &effectPath) {
  return mainChannel.addEffect(effectPath);
}

void MainChannelContainer::resetChannel() {
  mainChannel.reset();
}

} // Gui
} // Gj