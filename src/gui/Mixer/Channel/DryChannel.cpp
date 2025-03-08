//
// Created by ns on 3/2/25.
//

#include "DryChannel.h"

namespace Gj {
namespace Gui {

DryChannel::DryChannel(QWidget* parent)
  : QWidget(parent)
  , title(this)
  {

  title.setText("Dry");
  title.setFont({title.font().family(), 16});
  setStyle();
}

void DryChannel::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setStyleSheet("background-color: purple;");
}

} // Gui
} // Gj