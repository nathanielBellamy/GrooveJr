//
// Created by ns on 6/1/25.
//

#include "SceneButton.h"

namespace Gj {
namespace Gui {

SceneButton::SceneButton(QWidget* parent, QAction* action, const QString& title)
  : QPushButton(parent)
  , action(action)
  {
  setCursor(Qt::PointingHandCursor);
  setText(title);
  setStyle();
}

void SceneButton::setStyle() {
  setMinimumWidth(60);
}

void SceneButton::mousePressEvent(QMouseEvent* event) {
  action->activate(QAction::Trigger);
}

} // Gui
} // Gj
