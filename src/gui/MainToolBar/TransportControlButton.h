//
// Created by ns on 5/18/25.
//

#ifndef TRANSPORTCONTROLBUTTON_H
#define TRANSPORTCONTROLBUTTON_H

#include <QAction>
#include <QPushButton>

#include "../Color.h"

namespace Gj {
namespace Gui {

class TransportControlButton final : public QPushButton {

  public:
    TransportControlButton(QWidget* parent, QAction* action, QIcon icon);
    void setMute(float val);

  private:
    int channelIndex;
    QAction* action;
    std::string styleString(float muteVal) const;
    void setStyle();
    void mousePressEvent(QMouseEvent* event) override;
};

} // Gui
} // Gj


#endif //TRANSPORTCONTROLBUTTON_H
