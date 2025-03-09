//
// Created by ns on 3/8/25.
//

#ifndef ADDSWAPBUTTON_H
#define ADDSWAPBUTTON_H

#include <iostream>

#include <QPoint>
#include <QPushButton>

namespace Gj {
namespace Gui {

class AddSwapButton final : public QPushButton {

  public:
    AddSwapButton(QWidget* parent, int channelIndex, int slotIndex, bool occupied);

  private:
    int channelIndex;
    int slotIndex;
    bool occupied;
    void mousePressEvent(QMouseEvent* event) override;

};

} // Gui
} // Gj



#endif //ADDSWAPBUTTON_H
