//
// Created by ns on 3/8/25.
//

#include "SoloButton.h"

namespace Gj {
namespace Gui {

SoloButton::SoloButton(QWidget* parent, QAction* soloChannelAction, const int channelIndex)
  : QPushButton("&S", parent)
  , channelIndex(channelIndex)
  , soloChannelAction(soloChannelAction)
  {

  setStyle();
}

void SoloButton::setStyle() {
  setCursor(Qt::PointingHandCursor);
  setStyleSheet("background-color: grey; border: 2px solid white; border-radius: 5px; color: black;");
}

void SoloButton::mousePressEvent(QMouseEvent* event){
  soloChannelAction->setData(channelIndex);
  soloChannelAction->activate(QAction::Trigger);
}

void SoloButton::setSolo(const float val) {
  if (val == 1.0f) {
    setStyleSheet("background-color: yellow; border: 2px solid white; border-radius: 5px; color: black;");
  } else {
    setStyleSheet("background-color: grey; border: 2px solid white; border-radius: 5px; color: black;");
  }
}

} // Gui
} // Gj