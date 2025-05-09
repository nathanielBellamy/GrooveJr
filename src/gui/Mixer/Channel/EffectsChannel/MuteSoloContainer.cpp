//
// Created by ns on 3/8/25.
//

#include "MuteSoloContainer.h"

namespace Gj {
namespace Gui {

MuteSoloContainer::MuteSoloContainer(
  QWidget* parent,
  QAction* openEffectsContainer,
  QAction* muteChannelAction,
  QAction* soloChannelAction,
  const int channelIndex)
  : QWidget(parent)
  , channelIndex(channelIndex)
  , grid(this)
  , mute(this, muteChannelAction, channelIndex)
  , solo(this, soloChannelAction, channelIndex)
  , effects(this, openEffectsContainer)
  {

  setupGrid();
}

MuteSoloContainer::~MuteSoloContainer() {
  Logging::write(
    Info,
    "Gui::MuteSoloContainer::~MuteSoloContainer",
    "Destroying MuteSoloContainer"
  );
}

void MuteSoloContainer::hydrateState(const AppStatePacket &appState, int newChannelIdx) {
  channelIndex = newChannelIdx;
}

void MuteSoloContainer::setupGrid() {
  grid.setVerticalSpacing(0);
  grid.setHorizontalSpacing(2);

  grid.addWidget(&mute, 0, 0, 1, 1);
  grid.addWidget(&solo, 0, 1, 1, 1);
  grid.addWidget(&effects, 0, 2, 1, 1);
}

void MuteSoloContainer::setMute(const float val) {
  mute.setMute(val);
}

void MuteSoloContainer::setSolo(const float val) {
  solo.setSolo(val);
}

} // Gui
} // Gj