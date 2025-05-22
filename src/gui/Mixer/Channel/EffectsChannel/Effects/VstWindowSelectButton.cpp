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
  setFixedHeight(50);
}

void VstWindowSelectButton::mousePressEvent(QMouseEvent* event) {
  std::cout << "VstWindowSelectButton::mousePressEvent" << std::endl;
  selectAction->setData(effectIndex);
  selectAction->activate(QAction::Trigger);
}

} // Gui
} // Gj