//
// Created by ns on 4/29/25.
//

#ifndef ADDPLUGINBUTTON_H
#define ADDPLUGINBUTTON_H

#include <QAction>
#include <QPushButton>
#include <QWidget>

#include "../../../../Color.h"

namespace Gj {
namespace Gui {
namespace Mixer {
class AddPluginButton final : public QPushButton {
public:
  AddPluginButton(QWidget* parent, QAction* addEffectAction);

private:
  QAction* addPluginAction;

  void setStyle();

  void mousePressEvent(QMouseEvent* event) override;
};
} // Mixer
} // Gui
} // Gj


#endif //ADDPLUGINBUTTON_H
