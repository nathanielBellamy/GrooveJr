//
// Created by ns on 3/8/25.
//

#include "EffectSlot.h"

namespace Gj {
namespace Gui {

EffectSlot::EffectSlot(QWidget* parent, int channelIndex, int slotIndex, bool occupied)
  : QWidget(parent)
  , channelIndex(channelIndex)
  , slotIndex(slotIndex)
  , occupied(occupied)
  , grid(this)
  , title(this)
  , addSwapButton(this, occupied)
  , pluginName(this)
  {

  title.setText(QString::number(slotIndex + 1));
  title.setFont({title.font().family(), 12});
  setStyle();
  setupGrid();
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

void EffectSlot::mousePressEvent(QMouseEvent* event) {
  std::cout << "foo " << channelIndex << " " << slotIndex << std::endl;
}

} // Gui
} // Gj