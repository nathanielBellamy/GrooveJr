//
// Created by ns on 4/27/25.
//

#include "AddEffectsChannelButton.h"

namespace Gj {
namespace Gui {
namespace Mixer {
AddEffectsChannelButton::AddEffectsChannelButton(QWidget* parent, QAction* action)
: QPushButton("&+", parent)
  , addEffectsChannelAction(action) {
  setStyle();
}

void AddEffectsChannelButton::mousePressEvent(QMouseEvent* event) {
  addEffectsChannelAction->activate(QAction::Trigger);
}

void AddEffectsChannelButton::setStyle() {
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
