//
// Created by ns on 5/21/25.
//

#ifndef VSTWINDOWSELECTBUTTON_H
#define VSTWINDOWSELECTBUTTON_H

#include <QAction>
#include <QMouseEvent>
#include <QPushButton>

#include "../../../../../types/AtomicStr.h"
#include "../../../../../types/Types.h"

namespace Gj {
namespace Gui {
namespace Mixer {
class VstWindowSelectButton final : public QPushButton {
  PluginIndex effectIndex;
  QAction* selectAction;

  void mousePressEvent(QMouseEvent* event) override;

public:
  VstWindowSelectButton(QWidget* parent, PluginIndex effectIndex, const AtomicStr& pluginName, QAction* selectAction);
};
} // Mixer
} // Gui
} // Gj


#endif //VSTWINDOWSELECTBUTTON_H
