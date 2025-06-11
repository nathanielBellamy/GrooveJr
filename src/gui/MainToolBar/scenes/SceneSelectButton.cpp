//
// Created by ns on 6/1/25.
//

#include "SceneSelectButton.h"

namespace Gj {
namespace Gui {

SceneSelectButton::SceneSelectButton(QWidget* parent, QAction* action, const int sceneIndex)
  : QPushButton(parent)
  , sceneIndex(sceneIndex)
  , action(action)
  {
  setCursor(Qt::PointingHandCursor);
  setText(std::to_string(sceneIndex + 1).data());
  setStyle();
}

void SceneSelectButton::setStyle() {
  setFixedSize(sizeHint());
}

void SceneSelectButton::mousePressEvent(QMouseEvent* event) {
  action->setData(sceneIndex);
  action->activate(QAction::Trigger);
}

void SceneSelectButton::hydrateState(const AppStatePacket& appState) {
  std::string styleString;

  // TODO: why does setting colors affect button size?
  if (appState.sceneIndex == sceneIndex)
    styleString += "background-color: " + Color::toHex(GjC::LIGHT_300) +"; color: " + Color::toHex(GjC::DARK_400) + "; padding: 0px;";

  setStyleSheet(styleString.c_str());
  setStyle();

  if (appState.sceneIndex == sceneIndex || appState.playState == PLAY || appState.playState == FF || appState.playState == RW) {
    setEnabled(false);
  } else {
    setEnabled(true);
  }
}

QSize SceneSelectButton::sizeHint() const {
  return {40, 30};
}


} // Gui
} // Gj
