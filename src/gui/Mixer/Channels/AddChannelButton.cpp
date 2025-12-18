//
// Created by ns on 4/27/25.
//

#include "AddChannelButton.h"

namespace Gj {
namespace Gui {
namespace Mixer {
AddChannelButton::AddChannelButton(QWidget* parent, QAction* action)
: QPushButton("&+", parent)
  , addChannelAction(action) {
  setStyle();
}

void AddChannelButton::mousePressEvent(QMouseEvent* event) {
  addChannelAction->activate(QAction::Trigger);
}

void AddChannelButton::setStyle() {
  setCursor(Qt::PointingHandCursor);
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setFixedWidth(40);
  setStyleSheet(
    ("background-color: " + Color::toHex(GjC::DARK_400)).data()
  );
}
} // Mixer
} // Gui
} // Gj
