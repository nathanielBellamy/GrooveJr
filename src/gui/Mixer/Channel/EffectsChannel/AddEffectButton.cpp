//
// Created by ns on 4/29/25.
//

#include "AddEffectButton.h"

namespace Gj {
namespace Gui {

AddEffectButton::AddEffectButton(QWidget* parent, QAction* addEffectAction)
  : QPushButton("&+", parent)
  , addEffectAction(addEffectAction)
  {
  setStyle();
}

void AddEffectButton::mousePressEvent(QMouseEvent* event){
  addEffectAction->activate(QAction::Trigger);
}

void AddEffectButton::setStyle(){
  setCursor(Qt::PointingHandCursor);
  setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  setStyleSheet(
  ("min-width: 20px; min-height: 20px; max-height: 20px; background-color: " + Color::toHex(GjC::DARK_400)).data()
  );
}

} // Gui
} // Gj