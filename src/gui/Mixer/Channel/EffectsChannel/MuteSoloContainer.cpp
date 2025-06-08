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
  QAction* muteLChannelAction,
  QAction* muteRChannelAction,
  QAction* soloChannelAction,
  QAction* soloLChannelAction,
  QAction* soloRChannelAction,
  const int channelIndex)
  : QWidget(parent)
  , channelIndex(channelIndex)
  , grid(this)
  , mute(this, muteChannelAction, channelIndex)
  , muteL(this, muteLChannelAction, channelIndex)
  , muteR(this, muteRChannelAction, channelIndex)
  , solo(this, soloChannelAction, channelIndex)
  , soloL(this, soloLChannelAction, channelIndex)
  , soloR(this, soloRChannelAction, channelIndex)
  , effects(this, openEffectsContainer)
  {

  if (channelIndex == 0) {
    solo.hide();
    soloL.hide();
    soloR.hide();
  }

  setupGrid();
}

MuteSoloContainer::~MuteSoloContainer() {
  Logging::write(
    Info,
    "Gui::MuteSoloContainer::~MuteSoloContainer",
    "Destroying MuteSoloContainer on channelIndex: " + std::to_string(channelIndex)
  );
}

void MuteSoloContainer::hydrateState(const AppStatePacket &appState, int newChannelIdx) {
  channelIndex = newChannelIdx;
}

void MuteSoloContainer::setupGrid() {
  grid.setVerticalSpacing(0);
  grid.setHorizontalSpacing(2);

  grid.addWidget(&mute, 0, 0, 1, 2);
  grid.addWidget(&muteL, 1, 0, 1, 1);
  grid.addWidget(&muteR, 1, 1, 1, 1);
  if (channelIndex > 0) {
    grid.addWidget(&solo, 0, 2, 1, 2);
    grid.addWidget(&soloL, 1, 2, 1, 1);
    grid.addWidget(&soloR, 1, 3, 1, 1);
    grid.addWidget(&effects, 0, 4, 2, 1);
  } else {
    grid.addWidget(&effects, 0, 2, 2, 2);
  }

  grid.setHorizontalSpacing(4);
  grid.setVerticalSpacing(4);
}

void MuteSoloContainer::setMute(const float val) {
  mute.setMute(val);
}

void MuteSoloContainer::setMuteL(const float val) {
  muteL.setMute(val);
}

void MuteSoloContainer::setMuteR(const float val) {
  muteR.setMute(val);
}

void MuteSoloContainer::setSolo(const float val) {
  solo.setSolo(val);
}

void MuteSoloContainer::setSoloL(const float val) {
  soloL.setSolo(val);
}

void MuteSoloContainer::setSoloR(const float val) {
  soloR.setSolo(val);
}

} // Gui
} // Gj