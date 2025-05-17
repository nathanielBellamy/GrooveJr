//
// Created by ns on 3/25/25.
//

#include "EffectsButton.h"

namespace Gj {
namespace Gui {

EffectsButton::EffectsButton(QWidget* parent, QAction* openEffectsContainer)
  : QPushButton("&Fx", parent)
  , openEffectsContainer(openEffectsContainer)
  {
  setStyle();
}

void EffectsButton::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setCursor(Qt::PointingHandCursor);
  std::string styleString = "border: 2px solid white; border-radius: 5px; ";
  styleString += "background-color: " + Color::toHex(GjC::LIGHT_200) + "; ";
  styleString += "color: " + Color::toHex(GjC::DARK_200) + "; ";
  setStyleSheet(styleString.data());
}

void EffectsButton::mousePressEvent(QMouseEvent* event) {
  openEffectsContainer->activate(QAction::Trigger);
}

} // Gui
} // Gj
