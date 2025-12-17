//
// Created by ns on 3/8/25.
//

#include "MuteSoloContainer.h"

namespace Gj {
namespace Gui {
namespace Mixer {
MuteSoloContainer::MuteSoloContainer(
  QWidget* parent,
  Audio::Mixer::Core* mixer,
  const ChannelIndex channelIndex,
  QAction* openEffectsContainer,
  QAction* muteChannelAction,
  QAction* muteLChannelAction,
  QAction* muteRChannelAction,
  QAction* soloChannelAction,
  QAction* soloLChannelAction,
  QAction* soloRChannelAction
)
: QWidget(parent)
  , mixer(mixer)
  , channelIndex(channelIndex)
  , grid(this)
  , mute(this, muteChannelAction, channelIndex)
  , muteL(this, muteLChannelAction, channelIndex)
  , muteR(this, muteRChannelAction, channelIndex)
  , solo(this, soloChannelAction, channelIndex)
  , soloL(this, soloLChannelAction, channelIndex)
  , soloR(this, soloRChannelAction, channelIndex)
  , effects(this, openEffectsContainer) {
  const auto res = mixer->runAgainstChannel(
    channelIndex,
    [this, &channelIndex](const Audio::Mixer::Channel* channel) {
      mute.setMute(channel->settings.mute.load());
      muteL.setMute(channel->settings.muteL.load());
      muteR.setMute(channel->settings.muteR.load());
      if (channelIndex > 0) {
        solo.setSolo(channel->settings.solo.load());
        soloL.setSolo(channel->settings.soloL.load());
        soloR.setSolo(channel->settings.soloR.load());
      } else {
        solo.hide();
        soloL.hide();
        soloR.hide();
      }
    }
  );

  if (res != OK)
    Logging::write(
      res == WARNING ? Warning : Error,
      "Gui::MuteSoloContainer::MuteSoloContainer()",
      "An error occurred during construction on channelIndex: " + std::to_string(channelIndex)
    );

  setupGrid();
}

MuteSoloContainer::~MuteSoloContainer() {
  Logging::write(
    Info,
    "Gui::MuteSoloContainer::~MuteSoloContainer",
    "Destroying MuteSoloContainer on channelIndex: " + std::to_string(channelIndex)
  );
}

void MuteSoloContainer::hydrateState(const AppStatePacket& appState, const ChannelIndex newChannelIdx) {
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
} // Mixer
} // Gui
} // Gj
