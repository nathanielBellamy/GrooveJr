//
// Created by ns on 3/2/25.
//

#include "Channel.h"

namespace Gj {
namespace Gui {


Channel::Channel(QWidget* parent)
  : QWidget(parent)
  {

  setStyle();
}

void Channel::setStyle() {
  setStyleSheet("background-color: purple;");
}

} // Gui
} // Gj