//
// Created by ns on 5/21/25.
//

#include "VstWindowSelectButton.h"

namespace Gj {
namespace Gui {

VstWindowSelectButton::VstWindowSelectButton(QWidget* parent, const int effectIndex, std::string pluginName, QAction* selectAction)
  : QPushButton(pluginName.data(), parent)
  , effectIndex(effectIndex)
  , selectAction(selectAction)
  {
  setCursor(Qt::PointingHandCursor);
  setFixedHeight(40);
  setMinimumWidth(200);
  setStyleSheet("padding: 5px;");
}

void VstWindowSelectButton::mousePressEvent(QMouseEvent* event) {
  selectAction->setData(effectIndex);
  selectAction->activate(QAction::Trigger);
}

} // Gui
} // Gj