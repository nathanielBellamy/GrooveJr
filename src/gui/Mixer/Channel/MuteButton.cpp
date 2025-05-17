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

std::string MuteButton::styleString(const float muteVal) const {
  std::string styleString = "border-radius: 5px; border: 2px solid white;";
  if (muteVal == 1.0f) {
    styleString += " background-color: lightblue; ";
  } else {
    styleString += " background-color: "  + Color::toHex(GjC::LIGHT_200) + "; ";
  }
  return styleString;
};

void MuteButton::setMute(const float val) {
  setStyleSheet(styleString(val).data());
}


void MuteButton::setStyle() {
  setCursor(Qt::PointingHandCursor);
  setStyleSheet(styleString(false).data());
}

void MuteButton::mousePressEvent(QMouseEvent* event){
  muteChannelAction->setData(channelIndex);
  muteChannelAction->activate(QAction::Trigger);
}

} // Gui
} // Gj