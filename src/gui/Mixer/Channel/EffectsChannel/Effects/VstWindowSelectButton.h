//
// Created by ns on 5/21/25.
//

#ifndef VSTWINDOWSELECTBUTTON_H
#define VSTWINDOWSELECTBUTTON_H

#include <QAction>
#include <QMouseEvent>
#include <QPushButton>

namespace Gj {
namespace Gui {

class VstWindowSelectButton final : public QPushButton {
  int effectIndex;
  QAction* selectAction;
  void mousePressEvent(QMouseEvent* event) override;

  public:
    VstWindowSelectButton(QWidget* parent, int effectIndex, std::string pluginName, QAction* selectAction);
};

} // Gui
} // Gj


#endif //VSTWINDOWSELECTBUTTON_H
