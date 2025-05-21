//
// Created by ns on 5/21/25.
//

#include "VstWindowSelectButton.h"

namespace Gj {
namespace Gui {

VstWindowSelectButton::VstWindowSelectButton(QWidget* parent, const int effectIndex, QAction* selectAction)
  : QPushButton(parent)
  , effectIndex(effectIndex)
  , selectAction(selectAction)
  {
  setCursor(Qt::PointingHandCursor);
}

void VstWindowSelectButton::mousePressEvent(QMouseEvent* event) {
  selectAction->setData(effectIndex);
  selectAction->activate(QAction::Trigger);
}

} // Gui
} // Gj