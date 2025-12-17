//
// Created by ns on 3/25/25.
//

#include "PluginsButton.h"

namespace Gj {
namespace Gui {
namespace Mixer {
PluginsButton::PluginsButton(QWidget* parent, QAction* openEffectsContainer)
: QPushButton("&Fx", parent)
  , openEffectsContainer(openEffectsContainer) {
  setStyle();
}

void PluginsButton::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setCursor(Qt::PointingHandCursor);
  std::string styleString = "border-radius: 5px; border: 2px solid white; ";
  styleString += "background-color: " + Color::toHex(GjC::LIGHT_200) + "; ";
  setStyleSheet(styleString.data());
}

void PluginsButton::mousePressEvent(QMouseEvent* event) {
  openEffectsContainer->activate(QAction::Trigger);
}
} // Mixer
} // Gui
} // Gj
