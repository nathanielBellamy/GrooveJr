//
// Created by ns on 3/8/25.
//

#include "EffectsSlots.h"

namespace Gj {
namespace Gui {

EffectsSlots::EffectsSlots(QWidget* parent, actor_system& actorSystem, Audio::Mixer* mixer, int channelIndex, QAction* addEffectAction)
  : QWidget(parent)
  , actorSystem(actorSystem)
  , mixer(mixer)
  , channelIndex(channelIndex)
  , grid(this)
  {

  auto slot0 = std::make_unique<EffectSlot>(this, actorSystem, mixer, channelIndex, 0, addEffectAction, false);
  auto slot1 = std::make_unique<EffectSlot>(this, actorSystem, mixer, channelIndex, 1, addEffectAction, false);
  auto slot2 = std::make_unique<EffectSlot>(this, actorSystem, mixer, channelIndex, 2, addEffectAction, false);
  auto slot3 = std::make_unique<EffectSlot>(this, actorSystem, mixer, channelIndex, 3, addEffectAction, false);
  auto slot4 = std::make_unique<EffectSlot>(this, actorSystem, mixer, channelIndex, 4, addEffectAction, false);

  effectsSlots.push_back(std::move(slot0));
  effectsSlots.push_back(std::move(slot1));
  effectsSlots.push_back(std::move(slot2));
  effectsSlots.push_back(std::move(slot3));
  effectsSlots.push_back(std::move(slot4));

  setupGrid();
}

void EffectsSlots::setupGrid() {
  grid.setVerticalSpacing(0);

  int row = 0;
  for (auto &effectSlot : effectsSlots) {
    grid.addWidget(effectSlot.get(), row, 0, 1, 1);
    row++;
  }
}

} // Gui
} // Gj