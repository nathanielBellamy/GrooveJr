//
// Created by ns on 6/1/25.
//

#include "SceneSelectButton.h"

namespace Gj {
namespace Gui {

SceneSelectButton::SceneSelectButton(QWidget* parent, QAction* action, const int sceneIndex)
  : QPushButton(parent)
  , action(action)
  , sceneIndex(sceneIndex)
  {
  setCursor(Qt::PointingHandCursor);
  setText(std::to_string(sceneIndex + 1).data());
  setStyle();
}

void SceneSelectButton::setStyle() {
  setFixedSize(QSize(40, 40));
}

void SceneSelectButton::mousePressEvent(QMouseEvent* event) {
  action->setData(sceneIndex);
  action->activate(QAction::Trigger);
}

} // Gui
} // Gj
