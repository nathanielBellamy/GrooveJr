//
// Created by ns on 4/27/25.
//

#include "AddEffectsChannelButton.h"

namespace Gj {
namespace Gui {

AddEffectsChannelButton::AddEffectsChannelButton(QWidget* parent, QAction* action)
  : QPushButton("&+", parent)
  , addEffectsChannelAction(action)
  {
    setStyle();
}

void AddEffectsChannelButton::mousePressEvent(QMouseEvent* event){
  addEffectsChannelAction->activate(QAction::Trigger);
}

void AddEffectsChannelButton::setStyle(){
  setCursor(Qt::PointingHandCursor);
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setStyleSheet("min-width: 40px; max-width: 40px; background-color: pink;");
}

} // Gui
} // Gj