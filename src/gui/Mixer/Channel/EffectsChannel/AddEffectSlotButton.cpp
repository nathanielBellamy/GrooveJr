//
// Created by ns on 4/29/25.
//

#include "AddEffectSlotButton.h"

namespace Gj {
namespace Gui {

AddEffectSlotButton::AddEffectSlotButton(QWidget* parent, QAction* addEffectAction)
  : QPushButton("&+", parent)
  , addEffectAction(addEffectAction)
  {
  setStyle();
}

void AddEffectSlotButton::mousePressEvent(QMouseEvent* event){
  addEffectAction->activate(QAction::Trigger);
}

void AddEffectSlotButton::setStyle(){
  setCursor(Qt::PointingHandCursor);
  setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  setStyleSheet(
  ("min-width: 20px; min-height: 20px; max-height: 20px; background-color: " + Color::toHex(GjC::DARK_300)).data()
  );
}

} // Gui
} // Gj