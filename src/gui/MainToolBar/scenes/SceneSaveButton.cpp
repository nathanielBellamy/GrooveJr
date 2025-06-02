//
// Created by ns on 6/1/25.
//

#include "SceneSaveButton.h"

namespace Gj {
namespace Gui {

SceneSaveButton::SceneSaveButton(QWidget* parent, QAction* action)
  : QPushButton(parent)
  , action(action)
  {
  setCursor(Qt::PointingHandCursor);
  setText("Save");
  setStyle();
}

void SceneSaveButton::setStyle() {
  setFixedHeight(40);
  setMinimumWidth(60);
}

void SceneSaveButton::mousePressEvent(QMouseEvent* event) {
  action->activate(QAction::Trigger);
}

} // Gui
} // Gj
