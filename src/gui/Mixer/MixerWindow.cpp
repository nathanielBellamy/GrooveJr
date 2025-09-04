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
    &soloChannelAction, &soloLChannelAction, &soloRChannelAction,
    reinterpret_cast<std::atomic<float>*>(&vuBuffer)
  )
  , effectsChannelsContainer(
    this, actorSystem, mixer,
    &muteChannelAction, &muteLChannelAction, &muteRChannelAction,
    &soloChannelAction, &soloLChannelAction, &soloRChannelAction,
    reinterpret_cast<std::atomic<float>*>(&vuBuffer)
  ) {

  mixer->setSetVuRingBufferFunc(
    [this](jack_ringbuffer_t* vuRingBuffer) { setVuRingBuffer(vuRingBuffer); }
  );

  title.setText("Mixer");
  title.setFont({title.font().family(), 18});

  connectActions();
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setStyle();
  setupGrid();
}

Result MixerWindow::vuWorkerStart() {
  stopVuWorker.store(false);
  vuWorker = std::thread([this]() {
    while (!stopVuWorker.load()) {
      if (vuRingBuffer == nullptr)
        continue;

      if (jack_ringbuffer_read_space(vuRingBuffer) > Audio::VU_RING_BUFFER_SIZE - 2) {
        jack_ringbuffer_read(
          vuRingBuffer,
          reinterpret_cast<char*>(vuBufferIn),
          Audio::VU_RING_BUFFER_SIZE
        );
      }

      for (int i = 0; i < Audio::VU_RING_BUFFER_SIZE; i++) {
        vuBufferAvg[vuAvgIndex][i] = vuBufferIn[i];
        float avg = 0.0f;
        for (int j = 0; j < VU_METER_AVG_SIZE; j++)
          avg += vuBufferAvg[j][i];

        avg /= VU_METER_AVG_SIZE;
        vuBuffer[i].store(avg);
      }

      vuAvgIndex = (vuAvgIndex + 1) % VU_METER_AVG_SIZE;
    }
  });
  vuWorker.detach();

  return OK;
}

Result MixerWindow::vuWorkerStop() {
  stopVuWorker.store(true);

  return OK;
}

void MixerWindow::hydrateState(const AppStatePacket &appState) {
  mainChannelContainer.hydrateState(appState);
  effectsChannelsContainer.hydrateState(appState);

  if (appState.playState == PLAY || appState.playState == FF || appState.playState == RW)
    vuWorkerStart();
  else
    vuWorkerStop();
}

void MixerWindow::setStyle() {
  const std::string styleString = "background-color: " + Color::toHex(GjC::DARK_400) + ";";
  setStyleSheet(styleString.data());
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

void MixerWindow::setEffects() {
  Logging::write(
    Info,
    "Gui::MixerWindow::setEffects",
    "Setting effects."
  );

  mainChannelContainer.setEffects();
  effectsChannelsContainer.setEffects();
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

void MixerWindow::setChannels() {
  mainChannelContainer.setChannel();
  effectsChannelsContainer.setChannels();
}


} // Gui
} // Gj