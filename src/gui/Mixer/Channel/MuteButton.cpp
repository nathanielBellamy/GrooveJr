//
// Created by ns on 3/8/25.
//

#include "MuteButton.h"

namespace Gj {
namespace Gui {

MuteButton::MuteButton(QWidget* parent, QAction* muteChannelAction, const int channelIndex)
  : QPushButton("&M", parent)
  , channelIndex(channelIndex)
  , muteChannelAction(muteChannelAction)
  {

  setStyle();
}

void MuteButton::setStyle() {
  setCursor(Qt::PointingHandCursor);
  setStyleSheet("background-color: grey; border: 2px solid white; border-radius: 5px;");
}

void MuteButton::mousePressEvent(QMouseEvent* event){
  muteChannelAction->setData(channelIndex);
  muteChannelAction->activate(QAction::Trigger);
}

} // Gui
} // Gj