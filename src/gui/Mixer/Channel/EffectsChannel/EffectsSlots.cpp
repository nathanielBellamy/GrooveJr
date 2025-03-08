//
// Created by ns on 3/8/25.
//

#include "EffectsSlots.h"

namespace Gj {
namespace Gui {

EffectsSlots::EffectsSlots(QWidget* parent)
  : QWidget(parent)
  , grid(this)
  {

  auto slot1 = std::make_unique<EffectSlot>(this);
  auto slot2 = std::make_unique<EffectSlot>(this);
  auto slot3 = std::make_unique<EffectSlot>(this);
  auto slot4 = std::make_unique<EffectSlot>(this);
  auto slot5 = std::make_unique<EffectSlot>(this);

  effectsSlots.push_back(std::move(slot1));
  effectsSlots.push_back(std::move(slot2));
  effectsSlots.push_back(std::move(slot3));
  effectsSlots.push_back(std::move(slot4));
  effectsSlots.push_back(std::move(slot5));

  setupGrid();
}

void EffectsSlots::setupGrid() {
  int row = 0;
  for (auto &effectSlot : effectsSlots) {
    grid.addWidget(effectSlot.get(), row, 0, 1, 1);
    row++;
  }
}

} // Gui
} // Gj