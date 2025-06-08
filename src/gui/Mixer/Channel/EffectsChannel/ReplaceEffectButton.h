//
// Created by ns on 3/8/25.
//

#ifndef ADDSWAPBUTTON_H
#define ADDSWAPBUTTON_H

#include <QAction>
#include <QIcon>
#include <QPoint>
#include <QPushButton>
#include <QStyle>
#include <QVariant>

#include "../../../../AppState.h"
#include "../../../../Logging.h"

namespace Gj {
namespace Gui {

class ReplaceEffectButton final : public QPushButton {

  public:
    ReplaceEffectButton(QWidget* parent, int channelIndex, int slotIndex, bool occupied, QAction* action);
    ~ReplaceEffectButton() override;
    void hydrateState(const AppStatePacket& appState, int newChannelIdx);

  private:
    int channelIndex;
    int slotIndex;
    bool occupied;
    QAction* replaceEffectAction;
    void mousePressEvent(QMouseEvent* event) override;
    void setStyle();

};

} // Gui
} // Gj



#endif //ADDSWAPBUTTON_H
