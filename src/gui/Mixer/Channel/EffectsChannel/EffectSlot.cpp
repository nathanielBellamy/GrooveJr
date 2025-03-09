//
// Created by ns on 3/8/25.
//

#include "EffectSlot.h"

namespace Gj {
namespace Gui {

EffectSlot::EffectSlot(QWidget* parent, int channelIndex, int slotIndex)
  : QWidget(parent)
  , channelIndex(channelIndex)
  , slotIndex(slotIndex)
  , title(this)
  {

  title.setText("Effect --");
  title.setFont({title.font().family(), 12});
  setStyle();
}

void EffectSlot::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setMinimumSize(QSize(100, 50));
  setStyleSheet("background-color: blue; border: 2px solid white;");
}

} // Gui
} // Gj