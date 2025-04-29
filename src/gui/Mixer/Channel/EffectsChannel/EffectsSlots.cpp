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
  , addEffectSlotButton(this, addEffectAction)
  {
  setupGrid();
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