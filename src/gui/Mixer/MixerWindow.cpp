//
// Created by ns on 2/28/25.
//

#include "MixerWindow.h"

namespace Gj {
namespace Gui {

MixerWindow::MixerWindow(QWidget* parent, actor_system& actorSystem, Audio::Mixer* mixer)
  : QWidget(parent)
  , actorSystem(actorSystem)
  , mixer(mixer)
  , muteChannelAction(QIcon::fromTheme(QIcon::ThemeIcon::AudioVolumeMuted), tr("&Mute Channel"), this)
  , soloChannelAction(QIcon::fromTheme(QIcon::ThemeIcon::AudioInputMicrophone), tr("&Solo Channel"), this)
  , grid(this)
  , title(this)
  , mainChannelContainer(
    this, actorSystem, mixer,
    &muteChannelAction, &muteLChannelAction, &muteRChannelAction,
    &soloChannelAction, &soloLChannelAction, &soloRChannelAction
  )
  , effectsChannelsContainer(
    this, actorSystem, mixer,
    &muteChannelAction, &muteLChannelAction, &muteRChannelAction,
    &soloChannelAction, &soloLChannelAction, &soloRChannelAction
  ) {

  title.setText("Mixer");
  title.setFont({title.font().family(), 18});

  connectActions();
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setStyle();
  setupGrid();
}

void MixerWindow::hydrateState(const AppStatePacket &appStatePacket) {
  mainChannelContainer.hydrateState(appStatePacket);
  effectsChannelsContainer.hydrateState(appStatePacket);
}

void MixerWindow::setStyle() {
  setStyleSheet("background-color: green;");
}

void MixerWindow::setupGrid() {
  grid.setVerticalSpacing(1);

  grid.addWidget(&title, 0, 0, 1, -1);
  grid.addWidget(&mainChannelContainer, 1, 0, -1, 1);
  grid.addWidget(&effectsChannelsContainer, 1, 1, -1, 1);

  grid.setColumnStretch(0, 1);
  grid.setColumnStretch(1, 10);
  grid.setRowMinimumHeight(0, 20);
  grid.setRowStretch(1, 10);

  setLayout(&grid);
}

void MixerWindow::connectActions() {
  auto muteChannelConnection = connect(&muteChannelAction, &QAction::triggered, [&]() {
    const int channelIdx = muteChannelAction.data().toInt();

    const float newMute = mixer->getEffectsChannel(channelIdx)->toggleMute();
    if (channelIdx == 0) {
      mainChannelContainer.setMute(newMute);
    } else {
      effectsChannelsContainer.setMute(channelIdx, newMute);
    }
  });

  auto muteLChannelConnection = connect(&muteLChannelAction, &QAction::triggered, [&]() {
    const int channelIdx = muteLChannelAction.data().toInt();

    const float newMute = mixer->getEffectsChannel(channelIdx)->toggleMuteL();
    if (channelIdx == 0) {
      mainChannelContainer.setMuteL(newMute);
    } else {
      effectsChannelsContainer.setMuteL(channelIdx, newMute);
    }
  });

  auto muteRChannelConnection = connect(&muteRChannelAction, &QAction::triggered, [&]() {
    const int channelIdx = muteRChannelAction.data().toInt();

    const float newMute = mixer->getEffectsChannel(channelIdx)->toggleMuteR();
    if (channelIdx == 0) {
      mainChannelContainer.setMuteR(newMute);
    } else {
      effectsChannelsContainer.setMuteR(channelIdx, newMute);
    }
  });

  auto soloChannelConnection = connect(&soloChannelAction, &QAction::triggered, [&]() {
    const int channelIdx = soloChannelAction.data().toInt();

    const float newSolo = mixer->getEffectsChannel(channelIdx)->toggleSolo();
    if (channelIdx == 0) {
      mainChannelContainer.setSolo(newSolo);
    } else {
      effectsChannelsContainer.setSolo(channelIdx, newSolo);
    }
  });

  auto soloLChannelConnection = connect(&soloLChannelAction, &QAction::triggered, [&]() {
    const int channelIdx = soloLChannelAction.data().toInt();

    const float newSolo = mixer->getEffectsChannel(channelIdx)->toggleSoloL();
    if (channelIdx == 0) {
      mainChannelContainer.setSoloL(newSolo);
    } else {
      effectsChannelsContainer.setSoloL(channelIdx, newSolo);
    }
  });

  auto soloRChannelConnection = connect(&soloRChannelAction, &QAction::triggered, [&]() {
    const int channelIdx = soloRChannelAction.data().toInt();

    const float newSolo = mixer->getEffectsChannel(channelIdx)->toggleSoloR();
    if (channelIdx == 0) {
      mainChannelContainer.setSoloR(newSolo);
    } else {
      effectsChannelsContainer.setSoloR(channelIdx, newSolo);
    }
  });
}


} // Gui
} // Gj