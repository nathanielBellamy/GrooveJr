//
// Created by ns on 5/18/25.
//

#include "TransportControlButton.h"

namespace Gj {
namespace Gui {

TransportControlButton::TransportControlButton(QWidget* parent, QAction* action, QIcon icon)
  : QPushButton(parent)
  , action(action)
  {
  setIcon(icon);
  setStyle();
}

void TransportControlButton::setStyle() {
  setMinimumSize(QSize(40, 40));
  setMaximumSize(QSize(40, 40));
}

void TransportControlButton::mousePressEvent(QMouseEvent* event) {
  action->activate(QAction::Trigger);
}

} // Gui
} // Gj