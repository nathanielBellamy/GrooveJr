//
// Created by ns on 5/21/25.
//

#include "VstWindowSelectButton.h"


namespace Gj {
namespace Gui {

VstWindowSelectButton::VstWindowSelectButton(QWidget* parent, const EffectIndex effectIndex, const AtomicStr& pluginName, QAction* selectAction)
  : QPushButton(pluginName.c_str(), parent)
  , effectIndex(effectIndex)
  , selectAction(selectAction)
  {
  setCursor(Qt::PointingHandCursor);
  setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  setMinimumHeight(40);
  setMinimumWidth(200);
  setStyleSheet("padding: 5px;");
}

void VstWindowSelectButton::mousePressEvent(QMouseEvent* event) {
  selectAction->setData(static_cast<quint64>(effectIndex));
  selectAction->activate(QAction::Trigger);
}

} // Gui
} // Gj