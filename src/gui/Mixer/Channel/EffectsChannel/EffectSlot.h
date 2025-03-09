//
// Created by ns on 3/8/25.
//

#ifndef EFFECTSLOT_H
#define EFFECTSLOT_H

#include <iostream>

#include <QGridLayout>
#include <QLabel>
#include <QWidget>

#include "AddSwapButton.h"

namespace Gj {
namespace Gui {

class EffectSlot final : public QWidget {

  public:
    EffectSlot(QWidget* parent, int channelIndex, int slotIndex, bool occupied);

  private:
    int channelIndex;
    int slotIndex;
    bool occupied;
    QGridLayout grid;
    QLabel title;
    AddSwapButton addSwapButton;
    QLabel pluginName;
    void setStyle();
    void setupGrid();
};

} // Gui
} // Gj



#endif //EFFECTSLOT_H
