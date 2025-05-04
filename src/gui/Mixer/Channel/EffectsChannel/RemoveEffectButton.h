//
// Created by ns on 4/30/25.
//

#ifndef REMOVEEFFECTBUTTON_H
#define REMOVEEFFECTBUTTON_H

#include <QAction>
#include <QIcon>
#include <QPoint>
#include <QPushButton>
#include <QStyle>
#include <QVariant>

#include "../../../../AppState.h"

namespace Gj {
namespace Gui {

class RemoveEffectButton final : public QPushButton {

  public:
    RemoveEffectButton(QWidget* parent, int channelIndex, int slotIndex, bool occupied, QAction* action);
    void hydrateState(const AppStatePacket& appState, int newChannelIdx);

  private:
    int channelIndex;
    int slotIndex;
    bool occupied;
    QAction* removeEffectAction;
    void mousePressEvent(QMouseEvent* event) override;
    void setStyle();

};

} // Gui
} // Gj

#endif //REMOVEEFFECTBUTTON_H
