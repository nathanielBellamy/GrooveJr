//
// Created by ns on 3/7/25.
//

#include "EffectsChannelContainer.h"

namespace Gj {
namespace Gui {

EffectsChannelContainer::EffectsChannelContainer(QWidget* parent)
  : QWidget(parent)
  , grid(this)
  {

  auto effectsChannel = std::make_unique<EffectsChannel>(this, 1);
  channels.push_back(std::move(effectsChannel));

  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setStyle();
  setupGrid();
}

void EffectsChannelContainer::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setStyleSheet("background-color: yellow;");
}

void EffectsChannelContainer::setupGrid() {
  int col = 0;
  for (auto &effectsChannel : channels) {
    grid.addWidget(effectsChannel.get(), 1, col, -1, 1);
    col++;
  }
}

} // Gui
} // Gj