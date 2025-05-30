//
// Created by ns on 4/29/25.
//

#ifndef ADDEFFECTSLOTBUTTON_H
#define ADDEFFECTSLOTBUTTON_H

#include <QAction>
#include <QPushButton>
#include <QWidget>

#include "../../../Color.h"

namespace Gj {
namespace Gui {

class AddEffectButton final : public QPushButton {
  public:
    AddEffectButton(QWidget* parent, QAction* addEffectAction);

  private:
    QAction* addEffectAction;
    void setStyle();
    void mousePressEvent(QMouseEvent* event) override;
};

} // Gui
} // Gj



#endif //ADDEFFECTSLOTBUTTON_H
