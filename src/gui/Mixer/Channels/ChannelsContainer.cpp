//
// Created by ns on 3/7/25.
//

#include "ChannelsContainer.h"

#include "../../../audio/Constants.h"

namespace Gj {
namespace Gui {
namespace Mixer {
ChannelsContainer::ChannelsContainer(
  QWidget* parent,
  actor_system& actorSystem,
  Audio::Mixer::Core* mixer,
  QAction* muteChannelAction,
  QAction* muteLChannelAction,
  QAction* muteRChannelAction,
  QAction* soloChannelAction,
  QAction* soloLChannelAction,
  QAction* soloRChannelAction,
  std::atomic<float>* vuPtr
)
: QWidget(parent)
  , actorSystem(actorSystem)
  , mixer(mixer)
  , grid(this)
  , spacer(this)
  , channelsWidget(this)
  , channelsScrollArea(this)
  , channelsGrid(&channelsWidget)
  , addChannelAction(QIcon::fromTheme(QIcon::ThemeIcon::ListAdd), tr("&AddChannel"), this)
  , removeChannelAction(QIcon::fromTheme(QIcon::ThemeIcon::ListRemove), tr("&RemoveChannel"), this)
  , addChannelButton(this, &addChannelAction)
  , muteChannelAction(muteChannelAction)
  , muteLChannelAction(muteLChannelAction)
  , muteRChannelAction(muteRChannelAction)
  , soloChannelAction(soloChannelAction)
  , soloLChannelAction(soloLChannelAction)
  , soloRChannelAction(soloRChannelAction)
  , vuPtr(vuPtr) {
  setChannels();
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  connectActions();
  setStyle();
  setupGrid();

  Logging::write(
    Info,
    "Gui::ChannelsContainer::ChannelsContainer()",
    "Instantiated ChannelsContainer"
  );
}

void ChannelsContainer::hydrateState(const State::Packet& appState) {
  Logging::write(
    Info,
    "Gui::ChannelsContainer::hydrateState",
    "Hydrating channels state"
  );

  // TODO: can we always enable this too?
  if (appState.playState == PLAY || appState.playState == FF || appState.playState == RW)
    addChannelButton.setEnabled(false);
  else
    addChannelButton.setEnabled(true);

  for (int i = 0; i < channels.size(); i++)
    channels.at(i)->hydrateState(appState, i + 1);

  setupGrid();
  update();
}

void ChannelsContainer::addChannel() {
  if (channels.size() > Audio::MAX_MIXER_CHANNELS - 2)
    return;

  const ChannelIndex channelIndex = channels.size() + 1;

  const auto channel = new Channel(
    this, actorSystem, mixer, channelIndex, &removeChannelAction,
    muteChannelAction, muteLChannelAction, muteRChannelAction,
    soloChannelAction, soloLChannelAction, soloRChannelAction,
    &vuPtr[Audio::BfrIdx::VU::left(channelIndex)]
  );
  channels.push_back(channel);

  if (channels.size() > 1)
    channels.front()->updateShowRemoveChannelButton(true);
}

void ChannelsContainer::removeChannel(const ChannelIndex channelIdx) {
  Logging::write(
    Info,
    "Gui::ChannelsContainer::removeChannel",
    "Removing channel " + std::to_string(channelIdx)
  );

  const auto itrToRemove = std::find_if(channels.begin(), channels.end(), [&channelIdx](Channel* channel) {
    return channel->channelIndex == channelIdx;
  });

  if (itrToRemove != channels.end()) {
    Logging::write(
      Info,
      "Gui::ChannelsContainer::removeChannel",
      "Found channel to remove with channelIndex: " + std::to_string(channelIdx)
    );

    const int indexToRemove = std::distance(channels.begin(), itrToRemove);
    delete channels.at(indexToRemove);
    channels.erase(channels.begin() + indexToRemove);
  } else {
    Logging::write(
      Error,
      "Gui::ChannelsContainer::removeChannel",
      "Unable to find channel to remove with channelIndex: " + std::to_string(channelIdx)
    );
  }

  if (channels.size() == 1)
    channels.front()->updateShowRemoveChannelButton(false);

  Logging::write(
    Info,
    "Gui::ChannelsContainer::removeChannel",
    "Done removing channel " + std::to_string(channelIdx)
  );
}

void ChannelsContainer::clearChannels() {
  Logging::write(
    Info,
    "Gui::ChannelsContainer::clearChannels",
    "Clear channels."
  );

  for (const auto& channel: channels) {
    delete channel;
  }

  channels.clear();

  Logging::write(
    Info,
    "Gui::ChannelsContainer::clearChannels",
    "Done clearing channels."
  );
}

void ChannelsContainer::setChannels() {
  Logging::write(
    Info,
    "Gui::ChannelsContainer::setChannels",
    "Setting channels."
  );

  clearChannels();

  Logging::write(
    Info,
    "Gui::ChannelsContainer::setChannels",
    "Done removing channels - channelsCount: " + std::to_string(channels.size())
  );

  for (int i = 0; i < mixer->getChannelsCount(); i++)
    addChannel();

  Logging::write(
    Info,
    "Gui::ChannelsContainer::setChannels",
    "Done adding channels - channelsCount: " + std::to_string(channels.size()) + " mixer ecc: " + std::to_string(
      mixer->getChannelsCount())
  );

  if (channels.empty())
    addChannel();

  setupGrid();
  Logging::write(
    Info,
    "Gui::ChannelsContainer::setChannels",
    "Done setting channels."
  );
}

// void ChannelsContainer::setPlugins() const {
//   Logging::write(
//     Info,
//     "Gui::ChannelsContainer::setPlugins",
//     "Setting plugins"
//   );
//   for (const auto& channel: channels)
//     channel->setPlugins();
// }

void ChannelsContainer::connectActions() {
  auto addChannelConnection = connect(&addChannelAction, &QAction::triggered, [&]() {
    Logging::write(
      Info,
      "Gui::ChannelsContainer::addChannelAction trig",
      "Adding Channel"
    );
    strong_actor_ptr appStateManagerPtr = actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER);

    scoped_actor self{actorSystem};
    self->anon_send(
      actor_cast<actor>(appStateManagerPtr),
      mix_add_channel_a_v
    );

    addChannel();
    setupGrid();
    update();
  });

  auto removeChannelConnection = connect(&removeChannelAction, &QAction::triggered, [&]() {
    const int channelIdx = removeChannelAction.data().toInt();
    Logging::write(
      Info,
      "Gui::ChannelsContainer::removeChannelAction trig",
      "Removing Channel " + std::to_string(channelIdx)
    );
    removeChannel(channelIdx);

    strong_actor_ptr appStateManagerPtr = actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER);

    const scoped_actor self{actorSystem};
    self->anon_send(
      actor_cast<actor>(appStateManagerPtr),
      channelIdx,
      mix_remove_channel_a_v
    );
  });
}

void ChannelsContainer::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setStyleSheet(
    ("border-radius: 5px; background-color: " + Color::toHex(GjC::LIGHT_200)).data()
  );
  channelsWidget.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
}

void ChannelsContainer::setupGrid() {
  setupChannelsScrollArea();
  grid.addWidget(&addChannelButton, 0, 1, -1, 1);
  int col = 0;
  for (const auto& channel: channels) {
    channelsGrid.addWidget(channel, 0, col, -1, 1);
    col++;
  }
}

void ChannelsContainer::setupChannelsScrollArea() {
  channelsScrollArea.setMinimumWidth(500);
  channelsScrollArea.setMaximumWidth(1000);
  grid.addWidget(&channelsScrollArea, 0, 0, -1, 1);
  channelsScrollArea.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  channelsScrollArea.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  channelsScrollArea.setWidgetResizable(true);
  channelsScrollArea.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  channelsScrollArea.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  channelsScrollArea.setLayoutDirection(Qt::LeftToRight);
  channelsScrollArea.setWidget(&channelsWidget);
  channelsWidget.setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

void ChannelsContainer::setMute(const ChannelIndex channelIdx, const float val) const {
  if (!channelIdx)
    return;

  channels.at(channelIdx - 1)->setMute(val);
}

void ChannelsContainer::setMuteL(const ChannelIndex channelIdx, const float val) const {
  if (!channelIdx)
    return;

  channels.at(channelIdx - 1)->setMuteL(val);
}

void ChannelsContainer::setMuteR(const ChannelIndex channelIdx, const float val) const {
  if (!channelIdx)
    return;

  channels.at(channelIdx - 1)->setMuteR(val);
}

void ChannelsContainer::setSolo(const ChannelIndex channelIdx, const float val) const {
  if (!channelIdx)
    return;

  channels.at(channelIdx - 1)->setSolo(val);
}

void ChannelsContainer::setSoloL(const ChannelIndex channelIdx, const float val) const {
  if (!channelIdx)
    return;

  channels.at(channelIdx - 1)->setSoloL(val);
}

void ChannelsContainer::setSoloR(const ChannelIndex channelIdx, const float val) const {
  if (!channelIdx)
    return;

  channels.at(channelIdx - 1)->setSoloR(val);
}
} // Mixer
} // Gui
} // Gj
