//
// Created by ns on 3/25/25.
//

#include "EffectsButton.h"

namespace Gj {
namespace Gui {

EffectsButton::EffectsButton(QWidget* parent)
  : QPushButton("&Fx", parent)
  {
  setStyle();
}

void EffectsButton::setStyle() {
  setCursor(Qt::PointingHandCursor);
  setStyleSheet("background-color: green; border: 2px solid white; border-radius: 5px;");
}

} // Gui
} // Gj
