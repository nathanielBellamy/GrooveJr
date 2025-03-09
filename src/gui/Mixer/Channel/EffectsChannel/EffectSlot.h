//
// Created by ns on 3/8/25.
//

#ifndef EFFECTSLOT_H
#define EFFECTSLOT_H

#include <iostream>

#include <QAction>
#include <QGridLayout>
#include <QLabel>
#include <QWidget>

#include "AddSwapButton.h"
#include "../../../Shared/VstSelect.h"

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
    QAction addEffectAction;
    AddSwapButton addSwapButton;
    VstSelect vstSelect;
    QUrl vstUrl;
    QLabel pluginName;
    void setStyle();
    void setupGrid();
};

} // Gui
} // Gj



#endif //EFFECTSLOT_H
