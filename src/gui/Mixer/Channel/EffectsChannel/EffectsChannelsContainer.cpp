//
// Created by ns on 3/7/25.
//

#include "EffectsChannelsContainer.h"

namespace Gj {
namespace Gui {

EffectsChannelsContainer::EffectsChannelsContainer(
  QWidget* parent,
  actor_system& actorSystem,
  Audio::Mixer* mixer,
  QAction* muteChannelAction,
  QAction* soloChannelAction
  )
  : QWidget(parent)
  , actorSystem(actorSystem)
  , mixer(mixer)
  , grid(this)
  , spacer(this)
  , addEffectsChannelAction(QIcon::fromTheme(QIcon::ThemeIcon::ListAdd), tr("&AddEffectsChannel"), this)
  , removeEffectsChannelAction(QIcon::fromTheme(QIcon::ThemeIcon::ListRemove), tr("&RemoveEffectsChannel"), this)
  , addEffectsChannelButton(this, &addEffectsChannelAction)
  , muteChannelAction(muteChannelAction)
  , soloChannelAction(soloChannelAction)
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

  for (int i = 0; i < channels.size(); i++) {
    channels.at(i)->hydrateState(appState, i+1);
  }

  setupGrid();
  update();
}

void EffectsChannelsContainer::addEffectsChannel() {
  auto effectsChannel = new EffectsChannel(
    this, actorSystem, mixer, channels.size() + 1, &removeEffectsChannelAction, muteChannelAction, soloChannelAction
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

    // setupGrid();
    // update();
  });
}

void EffectsChannelsContainer::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setStyleSheet("background-color: yellow; border-radius: 5px;");
}

void EffectsChannelsContainer::setupGrid() {
  int col = 0;
  for (auto &effectsChannel : channels) {
    grid.addWidget(effectsChannel, 1, col, -1, 1);
    col++;
  }
  grid.addWidget(&addEffectsChannelButton, 1, col + 1, 1, 1);
  grid.addWidget(&spacer, 0, col, -1, -1);
}

void EffectsChannelsContainer::setMute(const int channelIdx, const float val) const {
  channels.at(channelIdx - 1)->setMute(val);
}

  void EffectsChannelsContainer::setSolo(const int channelIdx, const float val) const {
  channels.at(channelIdx - 1)->setSolo(val);
}

} // Gui
} // Gj