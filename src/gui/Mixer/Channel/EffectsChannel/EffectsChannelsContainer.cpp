//
// Created by ns on 3/7/25.
//

#include "EffectsChannelsContainer.h"

namespace Gj {
namespace Gui {

EffectsChannelsContainer::EffectsChannelsContainer(QWidget* parent, actor_system& actorSystem, Audio::Mixer* mixer)
  : QWidget(parent)
  , actorSystem(actorSystem)
  , mixer(mixer)
  , grid(this)
  , spacer(this)
  , addEffectsChannelAction(QIcon::fromTheme(QIcon::ThemeIcon::ListAdd), tr("&AddEffectsChannel"), this)
  , addEffectsChannelButton(this, &addEffectsChannelAction)
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

  const int channelsCountDiff = mixer->getEffectsChannelsCount() - static_cast<int>(channels.size());
  if (channelsCountDiff < 0) {
    channels.erase(channels.begin() + mixer->getEffectsChannelsCount(), channels.end());
  }

  for (const auto& channel : channels) {
    channel->hydrateState(appState);
  }

  setupGrid();
  update();
}


void EffectsChannelsContainer::addEffectsChannel() {
  auto effectsChannel = std::make_unique<EffectsChannel>(this, actorSystem, mixer, channels.size() + 1);
  channels.push_back(std::move(effectsChannel));
}

void EffectsChannelsContainer::connectActions() {
  connect(&addEffectsChannelAction, &QAction::triggered, [&]() {
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
}

void EffectsChannelsContainer::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setStyleSheet("background-color: yellow; border-radius: 5px;");
}

void EffectsChannelsContainer::setupGrid() {
  int col = 0;
  for (auto &effectsChannel : channels) {
    grid.addWidget(effectsChannel.get(), 1, col, -1, 1);
    col++;
  }
  grid.addWidget(&addEffectsChannelButton, 1, col + 1, 1, 1);
  grid.addWidget(&spacer, 0, col, -1, -1);
}

} // Gui
} // Gj