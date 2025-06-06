//
// Created by ns on 3/7/25.
//

#include "EffectsChannelsContainer.h"

#include "../../../../audio/Constants.h"

namespace Gj {
namespace Gui {

EffectsChannelsContainer::EffectsChannelsContainer(
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
  {

  for (int i = 0; i < mixer->getEffectsChannelsCount(); i++) {
    addEffectsChannel();
  }

  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  connectActions();
  setStyle();
  setupGrid();
}

void EffectsChannelsContainer::hydrateState(const AppStatePacket &appState) {
  Logging::write(
    Info,
    "Gui::EffectsChannelsContainer::hydrateState",
    "Hydrating effects channels state"
  );

  if (appState.playState == PLAY || appState.playState == FF || appState.playState == RW) {
    addEffectsChannelButton.setEnabled(false);
  } else {
    addEffectsChannelButton.setEnabled(true);
  }

  for (int i = 0; i < channels.size(); i++) {
    channels.at(i)->hydrateState(appState, i+1);
  }

  setupGrid();
  update();
}

void EffectsChannelsContainer::addEffectsChannel() {
  if (channels.size() > Audio::MAX_EFFECTS_CHANNELS - 2)
    return;

  const auto effectsChannel = new EffectsChannel(
    this, actorSystem, mixer, channels.size() + 1, &removeEffectsChannelAction,
    muteChannelAction, muteLChannelAction, muteRChannelAction,
    soloChannelAction, soloLChannelAction, soloRChannelAction
  );
  channels.push_back(effectsChannel);

  if (channels.size() > 1) {
    channels.front()->updateShowRemoveEffectsChannelButton(true);
  }
}

void EffectsChannelsContainer::removeEffectsChannel(const int channelIdx) {
  delete channels.at(channelIdx - 1);
  channels.erase(channels.begin() + channelIdx - 1);

  if (channels.size() == 1) {
    channels.front()->updateShowRemoveEffectsChannelButton(false);
  }
}

void EffectsChannelsContainer::setEffects() {
  Logging::write(
    Info,
    "Gui::EffectsChannelsContainer::addEffectToChannel",
    "Setting effects"
  );
  for (const auto& channel : channels) {
    channel->setEffects();
  }
}

void EffectsChannelsContainer::connectActions() {
  auto addEffectsChannelConnection = connect(&addEffectsChannelAction, &QAction::triggered, [&]() {
    Logging::write(
      Info,
      "Gui::EffectsChannelsContainer::addEffectsChannelAction trig",
      "Adding Effects Channel"
    );
    strong_actor_ptr appStateManagerPtr = actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER);

    scoped_actor self{ actorSystem };
    self->anon_send(
        actor_cast<actor>(appStateManagerPtr),
        mix_add_effects_channel_a_v
    );

    addEffectsChannel();
    setupGrid();
    update();
  });

  auto removeEfffectsChannelConnection = connect(&removeEffectsChannelAction, &QAction::triggered, [&]() {
    const int channelIdx = removeEffectsChannelAction.data().toInt();
    Logging::write(
      Info,
      "Gui::EffectsChannelsContainer::removeEffectsChannelAction trig",
      "Removing Effects Channel " + std::to_string(channelIdx)
    );
    removeEffectsChannel(channelIdx);

    strong_actor_ptr appStateManagerPtr = actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER);

    scoped_actor self{ actorSystem };
    self->anon_send(
        actor_cast<actor>(appStateManagerPtr),
        channelIdx,
        mix_remove_effects_channel_a_v
    );
  });
}

void EffectsChannelsContainer::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setStyleSheet(
    ("border-radius: 5px; background-color: " + Color::toHex(GjC::LIGHT_200)).data()
  );
  channelsWidget.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);
}

void EffectsChannelsContainer::setupGrid() {
  setupChannelsScrollArea();
  grid.addWidget(&addEffectsChannelButton, 0, 1, -1, 1);
  int col = 0;
  for (const auto &effectsChannel : channels) {
    channelsGrid.addWidget(effectsChannel, 0, col, -1, 1);
    col++;
  }
}

void EffectsChannelsContainer::setupChannelsScrollArea() {
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

void EffectsChannelsContainer::setMute(const int channelIdx, const float val) const {
  channels.at(channelIdx - 1)->setMute(val);
}

void EffectsChannelsContainer::setMuteL(const int channelIdx, const float val) const {
  channels.at(channelIdx - 1)->setMuteL(val);
}

void EffectsChannelsContainer::setMuteR(const int channelIdx, const float val) const {
  channels.at(channelIdx - 1)->setMuteR(val);
}

void EffectsChannelsContainer::setSolo(const int channelIdx, const float val) const {
  channels.at(channelIdx - 1)->setSolo(val);
}

void EffectsChannelsContainer::setSoloL(const int channelIdx, const float val) const {
  channels.at(channelIdx - 1)->setSoloL(val);
}

void EffectsChannelsContainer::setSoloR(const int channelIdx, const float val) const {
  channels.at(channelIdx - 1)->setSoloR(val);
}

void EffectsChannelsContainer::resetChannels() {
  Logging::write(
    Info,
    "Gui::EffectsChannelsContainer::resetChannels",
    "Resetting channels."
  );

  for (const auto& channel : channels) {
    delete channel;
  }
  channels.clear();

  for (int i = 0; i < mixer->getEffectsChannelsCount(); i++) {
    addEffectsChannel();
  }

  Logging::write(
    Info,
    "Gui::EffectsChannelsContainer::resetChannels",
    "Done resetting channels."
  );
}

} // Gui
} // Gj