//
// Created by ns on 4/27/25.
//

#ifndef ADDCHANNELBUTTON_H
#define ADDCHANNELBUTTON_H

#include <QAction>
#include <QPoint>
#include <QPushButton>

#include "../../Color.h"

namespace Gj {
namespace Gui {
namespace Mixer {
class AddChannelButton : public QPushButton {
public:
  AddChannelButton(QWidget* parent, QAction* action);

private:
  QAction* addChannelAction;

  void setStyle();

  void mousePressEvent(QMouseEvent* event) override;
};
} // Mixer
} // Gui
} // Gj


#endif //ADDCHANNELBUTTON_H
