//
// Created by ns on 5/21/25.
//

#include "VstWindowSelectButton.h"


namespace Gj {
namespace Gui {
namespace Mixer {
VstWindowSelectButton::VstWindowSelectButton(QWidget* parent, const PluginIndex pluginIndex,
                                             const AtomicStr& pluginName, QAction* selectAction)
: QPushButton(pluginName.c_str(), parent)
  , pluginIndex(pluginIndex)
  , selectAction(selectAction) {
  setCursor(Qt::PointingHandCursor);
  setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  setMinimumHeight(28);
  setMinimumWidth(160);
  setStyleSheet("padding: 3px; border-radius: 3px;");
}

void VstWindowSelectButton::mousePressEvent(QMouseEvent* event) {
  selectAction->setData(static_cast<quint64>(pluginIndex));
  selectAction->activate(QAction::Trigger);
}
} // Mixer
} // Gui
} // Gj
