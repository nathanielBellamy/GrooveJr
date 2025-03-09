//
// Created by ns on 3/8/25.
//

#include "EffectsSlots.h"

namespace Gj {
namespace Gui {

EffectsSlots::EffectsSlots(QWidget* parent, int channelIndex)
  : QWidget(parent)
  , channelIndex(channelIndex)
  , grid(this)
  {

  auto slot0 = std::make_unique<EffectSlot>(this, channelIndex, 0);
  auto slot1 = std::make_unique<EffectSlot>(this, channelIndex, 1);
  auto slot2 = std::make_unique<EffectSlot>(this, channelIndex, 2);
  auto slot3 = std::make_unique<EffectSlot>(this, channelIndex, 3);
  auto slot4 = std::make_unique<EffectSlot>(this, channelIndex, 4);

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