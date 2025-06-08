//
// Created by ns on 3/8/25.
//

#include "EffectsSlots.h"

namespace Gj {
namespace Gui {

EffectsSlots::EffectsSlots(QWidget* parent,
                           actor_system& actorSystem,
                           Audio::Mixer* mixer,
                           const int channelIndex,
                           QAction* replaceEffectAction,
                           QAction* removeEffectAction)
  : QWidget(parent)
  , actorSystem(actorSystem)
  , mixer(mixer)
  , channelIndex(channelIndex)
  , grid(this)
  , replaceEffectAction(replaceEffectAction)
  , removeEffectAction(removeEffectAction)
  {
  setupGrid();
}

EffectsSlots::~EffectsSlots() {
  Logging::write(
    Info,
    "Gui::EffectsSlots::~EffectsSlots",
    "Destroying EffectsSlots on Channel " + std::to_string(channelIndex)
  );
}

void EffectsSlots::hydrateState(const AppStatePacket& appState, const int newChannelIndex) {
  channelIndex = newChannelIndex;

  for (const auto& effectSlot : effectsSlots)
    effectSlot->hydrateState(appState, channelIndex);
}

void EffectsSlots::addEffectSlot() {
  auto slot = std::make_unique<EffectSlot>(
    this,
    actorSystem,
    mixer,
    channelIndex,
    effectsSlots.size(),
    false,
    replaceEffectAction,
    removeEffectAction
  );
  effectsSlots.push_back(std::move(slot));
  setupGrid();
  update();
}

void EffectsSlots::removeEffectSlot() {
  effectsSlots.pop_back();
  setupGrid();
  update();
}

void EffectsSlots::setupGrid() {
  grid.setVerticalSpacing(4);

  int row = 0;
  for (auto &effectSlot : effectsSlots) {
    grid.addWidget(effectSlot.get(), row, 0, 1, 1);
    row++;
  }
}

void EffectsSlots::reset() {
  Logging::write(
    Info,
    "Gui::EffectsSlots::reset",
    "Resetting EffectsSlots on Channel " + std::to_string(channelIndex)
  );

  effectsSlots.clear();

  Logging::write(
    Info,
    "Gui::EffectsSlots::reset",
    "Done resetting EffectsSlots on channel " + std::to_string(channelIndex)
  );
}

} // Gui
} // Gj