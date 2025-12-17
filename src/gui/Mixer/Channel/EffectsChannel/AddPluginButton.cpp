//
// Created by ns on 4/29/25.
//

#include "AddPluginButton.h"

namespace Gj {
namespace Gui {
namespace Mixer {
AddPluginButton::AddPluginButton(QWidget* parent, QAction* addEffectAction)
: QPushButton("&+", parent)
  , addPluginAction(addEffectAction) {
  setStyle();
}

void AddPluginButton::mousePressEvent(QMouseEvent* event) {
  addPluginAction->activate(QAction::Trigger);
}

void AddPluginButton::setStyle() {
  setCursor(Qt::PointingHandCursor);
  setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  setStyleSheet(
    ("min-width: 20px; min-height: 20px; max-height: 20px; background-color: " + Color::toHex(GjC::DARK_400)).data()
  );
}
} // Mixer
} // Gui
} // Gj
