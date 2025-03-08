//
// Created by ns on 3/8/25.
//

#include "MuteButton.h"

namespace Gj {
namespace Gui {

MuteButton::MuteButton(QWidget* parent)
  : QPushButton("&M", parent)
  {

  setStyle();
}

void MuteButton::setStyle() {
  setStyleSheet("background-color: grey; border: 2px solid white; border-radius: 5px;");
}

} // Gui
} // Gj