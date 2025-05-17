//
// Created by ns on 4/27/25.
//

#ifndef ADDEFFECTSCHANNELBUTTON_H
#define ADDEFFECTSCHANNELBUTTON_H

#include <QAction>
#include <QPoint>
#include <QPushButton>

#include "../../../Color.h"

namespace Gj {
namespace Gui {

class AddEffectsChannelButton : public QPushButton {
  public:
    AddEffectsChannelButton(QWidget* parent, QAction* action);

  private:
    QAction* addEffectsChannelAction;
    void setStyle();
    void mousePressEvent(QMouseEvent* event) override;
};

} // Gui
} // Gj


#endif //ADDEFFECTSCHANNELBUTTON_H
