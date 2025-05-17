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
  setStyleSheet(styleString(0.0f).data());
}

void SoloButton::mousePressEvent(QMouseEvent* event){
  soloChannelAction->setData(channelIndex);
  soloChannelAction->activate(QAction::Trigger);
}

std::string SoloButton::styleString(const float soloVal) const {
  std::string styleString = "border-radius: 5px; border: 2px solid white; ";
  if (soloVal == 1.0f) {
    styleString += "background-color: " + Color::toHex(GjC::SOLO_YELLOW) + "; color: " + Color::toHex(GjC::DARK_400) +"; ";
  } else {
    styleString += "background-color: " + Color::toHex(GjC::LIGHT_200) + "; ";
  }
  return styleString;
};

void SoloButton::setSolo(const float val) {
  setStyleSheet(styleString(val).data());
}

} // Gui
} // Gj