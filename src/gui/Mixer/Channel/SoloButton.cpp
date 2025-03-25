//
// Created by ns on 3/8/25.
//

#include "SoloButton.h"

namespace Gj {
namespace Gui {

SoloButton::SoloButton(QWidget* parent)
  : QPushButton("&S", parent)
  {

  setStyle();
}

void SoloButton::setStyle() {
  setCursor(Qt::PointingHandCursor);
  setStyleSheet("background-color: gold; border: 2px solid white; border-radius: 5px; color: black;");
}

} // Gui
} // Gj