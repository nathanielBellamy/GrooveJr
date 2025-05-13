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
  setStyleSheet("background-color: green; border: 2px solid white; border-radius: 5px;");
}

void EffectsButton::mousePressEvent(QMouseEvent* event) {
  openEffectsContainer->activate(QAction::Trigger);
}

} // Gui
} // Gj
