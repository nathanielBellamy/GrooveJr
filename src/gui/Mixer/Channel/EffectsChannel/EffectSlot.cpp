//
// Created by ns on 3/8/25.
//

#include "EffectSlot.h"

namespace Gj {
namespace Gui {

EffectSlot::EffectSlot(QWidget* parent, actor_system& actorSystem, Audio::Mixer* mixer, const int channelIndex, const int slotIndex, const bool occupied)
  : QWidget(parent)
  , actorSystem(actorSystem)
  , mixer(mixer)
  , channelIndex(channelIndex)
  , slotIndex(slotIndex)
  , occupied(occupied)
  , grid(this)
  , title(this)
  , addSwapButton(this, channelIndex, slotIndex, occupied, nullptr)
  , pluginName(this)
  {

  title.setText(QString::number(slotIndex + 1));
  title.setFont({title.font().family(), 12});
  pluginName.setFont({pluginName.font().family(), 12});

  setStyle();
  setupGrid();
}

void EffectSlot::hydrateState(const AppStatePacket& appState) {
  pluginName.setText("Hydrated");
  update();
}

void EffectSlot::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setMinimumSize(QSize(100, 50));
  setStyleSheet("background-color: blue; border: 2px solid white;");
}

void EffectSlot::setupGrid() {
  grid.addWidget(&title, 0, 0, 1, 1);
  grid.addWidget(&addSwapButton, 0, 1, 1, 1);
  grid.addWidget(&pluginName, 1, 0, -1, -1);

  grid.setColumnMinimumWidth(1, 30);
  grid.setColumnStretch(1, 10);

  grid.setVerticalSpacing(0);
  grid.setHorizontalSpacing(1);
}

} // Gui
} // Gj