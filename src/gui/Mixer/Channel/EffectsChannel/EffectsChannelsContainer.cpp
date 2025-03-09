//
// Created by ns on 3/7/25.
//

#include "EffectsChannelsContainer.h"

namespace Gj {
namespace Gui {

EffectsChannelsContainer::EffectsChannelsContainer(QWidget* parent, actor_system& actorSystem)
  : QWidget(parent)
  , actorSystem(actorSystem)
  , spacer(this)
  , grid(this)
  {

  auto effectsChannel1 = std::make_unique<EffectsChannel>(this, actorSystem, 1);
  channels.push_back(std::move(effectsChannel1));
  auto effectsChannel2 = std::make_unique<EffectsChannel>(this, actorSystem, 2);
  channels.push_back(std::move(effectsChannel2));

  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setStyle();
  setupGrid();
}

void EffectsChannelsContainer::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setStyleSheet("background-color: yellow;");
}

void EffectsChannelsContainer::setupGrid() {
  int col = 0;
  for (auto &effectsChannel : channels) {
    grid.addWidget(effectsChannel.get(), 1, col, -1, 1);
    col++;
  }
  grid.addWidget(&spacer, 0, col, -1, -1);
}

} // Gui
} // Gj