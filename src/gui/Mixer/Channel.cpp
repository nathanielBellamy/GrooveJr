//
// Created by ns on 3/2/25.
//

#include "Channel.h"

namespace Gj {
namespace Gui {

Channel::Channel(QWidget* parent, int index)
  : QWidget(parent)
  , title(this)
  , index(index)
  {

  title.setText("Channel " + QString::number(index));
  title.setFont({title.font().family(), 16});
  setStyle();
}

void Channel::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setStyleSheet("background-color: purple;");
}

} // Gui
} // Gj