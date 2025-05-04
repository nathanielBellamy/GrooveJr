//
// Created by ns on 3/8/25.
//

#include "EffectsSlots.h"

namespace Gj {
namespace Gui {

EffectsSlots::EffectsSlots(QWidget* parent,
                           actor_system& actorSystem,
                           Audio::Mixer* mixer,
                           int channelIndex,
                           QAction* addEffectAction,
                           QAction* replaceEffectAction,
                           QAction* removeEffectAction)
  : QWidget(parent)
  , actorSystem(actorSystem)
  , mixer(mixer)
  , channelIndex(channelIndex)
  , grid(this)
  , addEffectSlotButton(this, addEffectAction)
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

void EffectsSlots::hydrateState(const AppStatePacket& appState) const {
  for (const auto& effectSlot : effectsSlots) {
    effectSlot->hydrateState(appState);
  }
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
  grid.setVerticalSpacing(0);

  int row = 0;
  for (auto &effectSlot : effectsSlots) {
    grid.addWidget(effectSlot.get(), row, 0, 1, 1);
    row++;
  }
  grid.addWidget(&addEffectSlotButton, row, 0, 1, 1);
}

} // Gui
} // Gj