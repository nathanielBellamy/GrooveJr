//
// Created by ns on 5/3/25.
//

#ifndef REMOVEEFFECTSCHANNELBUTTON_H
#define REMOVEEFFECTSCHANNELBUTTON_H

#include <QAction>
#include <QIcon>
#include <QPoint>
#include <QPushButton>
#include <QStyle>
#include <QVariant>

namespace Gj {
namespace Gui {

class RemoveEffectsChannelButton final : public QPushButton {

  public:
    RemoveEffectsChannelButton(QWidget* parent, int channelIndex, QAction* action);

  private:
    int channelIndex;
    bool occupied;
    QAction* removeEffectsChannelAction;
    void mousePressEvent(QMouseEvent* event) override;
    void setStyle();

};

} // Gui
} // Gj

#endif //REMOVEEFFECTSCHANNELBUTTON_H
