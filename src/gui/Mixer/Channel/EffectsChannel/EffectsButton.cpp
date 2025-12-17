//
// Created by ns on 3/25/25.
//

#include "EffectsButton.h"

namespace Gj {
namespace Gui {
namespace Mixer {
EffectsButton::EffectsButton(QWidget* parent, QAction* openEffectsContainer)
: QPushButton("&Fx", parent)
  , openEffectsContainer(openEffectsContainer) {
  setStyle();
}

void EffectsButton::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setCursor(Qt::PointingHandCursor);
  std::string styleString = "border-radius: 5px; border: 2px solid white; ";
  styleString += "background-color: " + Color::toHex(GjC::LIGHT_200) + "; ";
  setStyleSheet(styleString.data());
}

void EffectsButton::mousePressEvent(QMouseEvent* event) {
  openEffectsContainer->activate(QAction::Trigger);
}
} // Mixer
} // Gui
} // Gj
