//
// Created by ns on 2/28/25.
//

#include "Body.h"

namespace Gj {
namespace Gui {
namespace Mixer {
Body::Body(QWidget* parent, actor_system& actorSystem, Audio::Mixer::Core* mixer)
: QWidget(parent)
  , actorSystem(actorSystem)
  , mixer(mixer)
  , muteChannelAction(new QAction(QIcon::fromTheme(QIcon::ThemeIcon::AudioVolumeMuted), tr("&Mute Channel"), this))
  , muteLChannelAction(new QAction(QIcon::fromTheme(QIcon::ThemeIcon::AudioVolumeMuted), tr("&Mute L Channel"), this))
  , muteRChannelAction(new QAction(QIcon::fromTheme(QIcon::ThemeIcon::AudioVolumeMuted), tr("&Mute R Channel"), this))
  , soloChannelAction(new QAction(QIcon::fromTheme(QIcon::ThemeIcon::AudioInputMicrophone), tr("&Solo Channel"), this))
  , soloLChannelAction(new QAction(QIcon::fromTheme(QIcon::ThemeIcon::AudioInputMicrophone), tr("&Solo L Channel"),
                                   this))
  , soloRChannelAction(new QAction(QIcon::fromTheme(QIcon::ThemeIcon::AudioInputMicrophone), tr("&Solo R Channel"),
                                   this))
  , grid(new QGridLayout(this))
  , title(new QLabel(this))
  , mainChannelContainer(new MainChannelContainer(
      this, actorSystem, mixer,
      muteChannelAction, muteLChannelAction, muteRChannelAction,
      soloChannelAction, soloLChannelAction, soloRChannelAction,
      reinterpret_cast<std::atomic<float>*>(&vuBuffer))
  )
  , channelsContainer(new ChannelsContainer(
      this, actorSystem, mixer,
      muteChannelAction, muteLChannelAction, muteRChannelAction,
      soloChannelAction, soloLChannelAction, soloRChannelAction,
      reinterpret_cast<std::atomic<float>*>(&vuBuffer))
  )
  , vuRingBuffer(nullptr) {
  mixer->setSetVuRingBufferFunc(
    [this](jack_ringbuffer_t* vuRingBuffer) { setVuRingBuffer(vuRingBuffer); }
  );

  title->setText("Mixer");
  title->setFont({title->font().family(), 18});

  connectActions();
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setStyle();
  setupGrid();

  Logging::write(
    Info,
    "Gui::Body::Body()",
    "Instantiated Body"
  );
}

Body::~Body() {
  vuWorkerStop();

  delete channelsContainer;
  delete mainChannelContainer;
  delete title;
  delete grid;
  delete soloRChannelAction;
  delete soloLChannelAction;
  delete soloChannelAction;
  delete muteRChannelAction;
  delete muteLChannelAction;
  delete muteChannelAction;
}

Result Body::vuWorkerStart() {
  stopVuWorker.store(false);
  vuWorker = std::thread([this]() {
    vuWorkerRunning = true;
    while (!stopVuWorker) {
      if (vuRingBuffer == nullptr)
        continue;

      if (jack_ringbuffer_read_space(vuRingBuffer) > Audio::VU_RING_BUFFER_SIZE - 2) {
        jack_ringbuffer_read(
          vuRingBuffer,
          reinterpret_cast<char*>(vuBufferIn),
          Audio::VU_RING_BUFFER_SIZE
        );
      }

      for (int i = 0; i < Audio::VU_BUFFER_SIZE; i++) {
        vuBufferAvg[vuAvgIndex][i] = vuBufferIn[i];
        float avg = 0.0f;
        for (int j = 0; j < VU_METER_AVG_SIZE; j++)
          avg += 20 * std::log10f(vuBufferAvg[j][i]);

        avg /= VU_METER_AVG_SIZE;
        vuBuffer[i].store(avg);
      }

      // std::cout << " vuBufferIn - " << vuBufferIn[0] << " - " << vuBufferIn[1] << std::endl;
      // std::cout << " vuBuffer   - " << vuBuffer[0].load() << " - " << vuBuffer[1].load() << std::endl;
      vuAvgIndex = (vuAvgIndex + 1) % VU_METER_AVG_SIZE;
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    vuWorkerRunning = false;
  });
  vuWorker.detach();

  return OK;
}

Result Body::vuWorkerStop() {
  stopVuWorker.store(true);
  return OK;
}

void Body::hydrateState(const State::Packet& appState) {
  mainChannelContainer->hydrateState(appState);
  channelsContainer->hydrateState(appState);

  if (appState.playState == PLAY || appState.playState == FF || appState.playState == RW) {
    if (!vuWorkerRunning)
      vuWorkerStart();
  } else {
    vuWorkerStop();
  }
}

void Body::setStyle() {
  const std::string styleString = "background-color: " + Color::toHex(GjC::DARK_400) + ";";
  setStyleSheet(styleString.data());
}

void Body::setupGrid() {
  grid->setVerticalSpacing(1);

  grid->addWidget(title, 0, 0, 1, -1);
  grid->addWidget(mainChannelContainer, 1, 0, -1, 1);
  grid->addWidget(channelsContainer, 1, 1, -1, 1);

  grid->setColumnStretch(0, 1);
  grid->setColumnStretch(1, 10);
  grid->setRowMinimumHeight(0, 20);
  grid->setRowStretch(1, 10);

  setLayout(grid);
}

// void Body::setPlugins() {
//   Logging::write(
//     Info,
//     "Gui::Body::setPlugins",
//     "Setting plugins."
//   );
//
//   mainChannelContainer->setPlugins();
//   channelsContainer->setPlugins();
// }
//

void Body::connectActions() {
  auto muteChannelConnection = connect(muteChannelAction, &QAction::triggered, [&]() {
    const ChannelIndex channelIdx = muteChannelAction->data().toULongLong();

    const auto res = mixer->runAgainstChannel(channelIdx, [this, &channelIdx](Audio::Mixer::Channel* channel) {
      const float newMute = channel->toggleMute();
      if (channelIdx == 0) {
        mainChannelContainer->setMute(newMute);
      } else {
        channelsContainer->setMute(channelIdx, newMute);
      }
    });

    if (res != OK)
      Logging::write(
        res == WARNING ? Warning : Error,
        "Gui::Body::connectActions::muteChannelAction",
        "Failed to update mute for channelIndex: " + std::to_string(channelIdx)
      );
  });

  auto muteLChannelConnection = connect(muteLChannelAction, &QAction::triggered, [&]() {
    const ChannelIndex channelIdx = muteLChannelAction->data().toULongLong();

    const auto res = mixer->runAgainstChannel(channelIdx, [this, &channelIdx](Audio::Mixer::Channel* channel) {
      const float newMute = channel->toggleMuteL();
      if (channelIdx == 0) {
        mainChannelContainer->setMuteL(newMute);
      } else {
        channelsContainer->setMuteL(channelIdx, newMute);
      }
    });

    if (res != OK)
      Logging::write(
        res == WARNING ? Warning : Error,
        "Gui::Body::connectActions::muteLChannelAction",
        "Failed to update muteL for channelIndex: " + std::to_string(channelIdx)
      );
  });

  auto muteRChannelConnection = connect(muteRChannelAction, &QAction::triggered, [&]() {
    const ChannelIndex channelIdx = muteRChannelAction->data().toULongLong();

    const auto res = mixer->runAgainstChannel(channelIdx, [this, &channelIdx](Audio::Mixer::Channel* channel) {
      const float newMute = channel->toggleMuteR();
      if (channelIdx == 0) {
        mainChannelContainer->setMuteR(newMute);
      } else {
        channelsContainer->setMuteR(channelIdx, newMute);
      }
    });

    if (res != OK)
      Logging::write(
        res == WARNING ? Warning : Error,
        "Gui::Body::connectActions::muteRChannelAction",
        "Failed to update muteR for channelIndex: " + std::to_string(channelIdx)
      );
  });

  auto soloChannelConnection = connect(soloChannelAction, &QAction::triggered, [&]() {
    const ChannelIndex channelIdx = soloChannelAction->data().toULongLong();

    const auto res = mixer->runAgainstChannel(channelIdx, [this, &channelIdx](Audio::Mixer::Channel* channel) {
      const float newSolo = channel->toggleSolo();
      if (channelIdx == 0) {
        mainChannelContainer->setSolo(newSolo);
      } else {
        channelsContainer->setSolo(channelIdx, newSolo);
      }
    });

    if (res != OK)
      Logging::write(
        res == WARNING ? Warning : Error,
        "Gui::Body::connectActions::muteRChannelAction",
        "Failed to update solo for channelIndex: " + std::to_string(channelIdx)
      );
  });

  auto soloLChannelConnection = connect(soloLChannelAction, &QAction::triggered, [&]() {
    const ChannelIndex channelIdx = soloLChannelAction->data().toULongLong();

    const auto res = mixer->runAgainstChannel(channelIdx, [this, &channelIdx](Audio::Mixer::Channel* channel) {
      const float newSolo = channel->toggleSoloL();
      if (channelIdx == 0) {
        mainChannelContainer->setSoloL(newSolo);
      } else {
        channelsContainer->setSoloL(channelIdx, newSolo);
      }
    });

    if (res != OK)
      Logging::write(
        res == WARNING ? Warning : Error,
        "Gui::Body::connectActions::soloLChannelAction",
        "Failed to update soloL for channelIndex: " + std::to_string(channelIdx)
      );
  });

  auto soloRChannelConnection = connect(soloRChannelAction, &QAction::triggered, [&]() {
    const ChannelIndex channelIdx = soloRChannelAction->data().toInt();

    const auto res = mixer->runAgainstChannel(channelIdx, [this, &channelIdx](Audio::Mixer::Channel* channel) {
      if (const auto channel = mixer->getChannel(channelIdx); channel) {
        const float newSolo = channel.value()->toggleSoloR();
        if (channelIdx == 0) {
          mainChannelContainer->setSoloR(newSolo);
        } else {
          channelsContainer->setSoloR(channelIdx, newSolo);
        }
      }
    });

    if (res != OK)
      Logging::write(
        res == WARNING ? Warning : Error,
        "Gui::Body::connectActions::soloLChannelAction",
        "Failed to update soloR for channelIndex: " + std::to_string(channelIdx)
      );
  });
}


void Body::setChannels() {
  mainChannelContainer->setChannel();
  channelsContainer->setChannels();
}
} // Mixer
} // Gui
} // Gj
