//
// Created by ns on 3/7/25.
//

#include "EffectsChannel.h"

namespace Gj {
namespace Gui {

EffectsChannel::EffectsChannel(QWidget* parent, int index)
  : QWidget(parent)
  , title(this)
  , index(index)
  {

  title.setText("FX " + QString::number(index));
  title.setFont({title.font().family(), 16});
  setStyle();
}

void EffectsChannel::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setStyleSheet("background-color: orange;");
}

} // Gui
} // Gj