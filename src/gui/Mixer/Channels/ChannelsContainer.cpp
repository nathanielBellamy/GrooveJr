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
  , addEffectsChannelAction(QIcon::fromTheme(QIcon::ThemeIcon::ListAdd), tr("&AddEffectsChannel"), this)
  , removeEffectsChannelAction(QIcon::fromTheme(QIcon::ThemeIcon::ListRemove), tr("&RemoveEffectsChannel"), this)
  , addEffectsChannelButton(this, &addEffectsChannelAction)
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
    "Gui::EffectsChannelsContainer::EffectsChannelsContainer()",
    "Instantiated EffectsChannelsContainer"
  );
}

void ChannelsContainer::hydrateState(const AppStatePacket& appState) {
  Logging::write(
    Info,
    "Gui::EffectsChannelsContainer::hydrateState",
    "Hydrating effects channels state"
  );

  if (appState.playState == PLAY || appState.playState == FF || appState.playState == RW)
    addEffectsChannelButton.setEnabled(false);
  else
    addEffectsChannelButton.setEnabled(true);

  for (int i = 0; i < channels.size(); i++)
    channels.at(i)->hydrateState(appState, i + 1);

  setupGrid();
  update();
}

void ChannelsContainer::addEffectsChannel() {
  if (channels.size() > Audio::MAX_MIXER_CHANNELS - 2)
    return;

  const ChannelIndex channelIndex = channels.size() + 1;

  const auto effectsChannel = new Channel(
    this, actorSystem, mixer, channelIndex, &removeEffectsChannelAction,
    muteChannelAction, muteLChannelAction, muteRChannelAction,
    soloChannelAction, soloLChannelAction, soloRChannelAction,
    &vuPtr[2 * channelIndex]
  );
  channels.push_back(effectsChannel);

  if (channels.size() > 1)
    channels.front()->updateShowRemoveEffectsChannelButton(true);
}

void ChannelsContainer::removeEffectsChannel(const ChannelIndex channelIdx) {
  Logging::write(
    Info,
    "Gui::EffectsChannelsContainer::removeEffectsChannel",
    "Removing effects channels channel " + std::to_string(channelIdx)
  );

  const auto itrToRemove = std::find_if(channels.begin(), channels.end(), [&channelIdx](Channel* channel) {
    return channel->channelIndex == channelIdx;
  });

  if (itrToRemove != channels.end()) {
    Logging::write(
      Info,
      "Gui::EffectsChannelsContainer::removeEffectsChannel",
      "Found channel to remove with channelIndex: " + std::to_string(channelIdx)
    );

    const int indexToRemove = std::distance(channels.begin(), itrToRemove);
    delete channels.at(indexToRemove);
    channels.erase(channels.begin() + indexToRemove);
  } else {
    Logging::write(
      Error,
      "Gui::EffectsChannelsContainer::removeEffectsChannel",
      "Unable to find channel to remove with channelIndex: " + std::to_string(channelIdx)
    );
  }

  if (channels.size() == 1)
    channels.front()->updateShowRemoveEffectsChannelButton(false);

  Logging::write(
    Info,
    "Gui::EffectsChannelsContainer::removeEffectsChannel",
    "Done removing effects channels channel " + std::to_string(channelIdx)
  );
}

void ChannelsContainer::clearEffectsChannels() {
  Logging::write(
    Info,
    "Gui::EffectsChannelsContainer::clearEffectsChannels",
    "Clear effects channels."
  );

  for (const auto& channel: channels) {
    delete channel;
  }

  channels.clear();

  Logging::write(
    Info,
    "Gui::EffectsChannelsContainer::clearEffectsChannels",
    "Done clearing effects channels."
  );
}

void ChannelsContainer::setChannels() {
  Logging::write(
    Info,
    "Gui::EffectsChannelsContainer::setChannels",
    "Setting channels."
  );

  clearEffectsChannels();

  Logging::write(
    Info,
    "Gui::EffectsChannelsContainer::setChannels",
    "Done removing channels - channelsCount: " + std::to_string(channels.size())
  );

  for (int i = 0; i < mixer->getEffectsChannelsCount(); i++)
    addEffectsChannel();

  Logging::write(
    Info,
    "Gui::EffectsChannelsContainer::setChannels",
    "Done adding channels - channelsCount: " + std::to_string(channels.size()) + " mixer ecc: " + std::to_string(
      mixer->getEffectsChannelsCount())
  );

  if (channels.empty())
    addEffectsChannel();

  setupGrid();
  Logging::write(
    Info,
    "Gui::EffectsChannelsContainer::setChannels",
    "Done setting channels."
  );
}

void ChannelsContainer::setEffects() const {
  Logging::write(
    Info,
    "Gui::EffectsChannelsContainer::addEffectToChannel",
    "Setting effects"
  );
  for (const auto& channel: channels)
    channel->setEffects();
}

void ChannelsContainer::connectActions() {
  auto addEffectsChannelConnection = connect(&addEffectsChannelAction, &QAction::triggered, [&]() {
    Logging::write(
      Info,
      "Gui::EffectsChannelsContainer::addEffectsChannelAction trig",
      "Adding Effects Channel"
    );
    strong_actor_ptr appStateManagerPtr = actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER);

    scoped_actor self{actorSystem};
    self->anon_send(
      actor_cast<actor>(appStateManagerPtr),
      mix_add_effects_channel_a_v
    );

    addEffectsChannel();
    setupGrid();
    update();
  });

  auto removeEffectsChannelConnection = connect(&removeEffectsChannelAction, &QAction::triggered, [&]() {
    const int channelIdx = removeEffectsChannelAction.data().toInt();
    Logging::write(
      Info,
      "Gui::EffectsChannelsContainer::removeEffectsChannelAction trig",
      "Removing Effects Channel " + std::to_string(channelIdx)
    );
    removeEffectsChannel(channelIdx);

    strong_actor_ptr appStateManagerPtr = actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER);

    const scoped_actor self{actorSystem};
    self->anon_send(
      actor_cast<actor>(appStateManagerPtr),
      channelIdx,
      mix_remove_effects_channel_a_v
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
  grid.addWidget(&addEffectsChannelButton, 0, 1, -1, 1);
  int col = 0;
  for (const auto& effectsChannel: channels) {
    channelsGrid.addWidget(effectsChannel, 0, col, -1, 1);
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
