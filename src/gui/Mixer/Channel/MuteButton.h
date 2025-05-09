//
// Created by ns on 3/8/25.
//

#ifndef MUTEBUTTON_H
#define MUTEBUTTON_H

#include <QAction>
#include <QPushButton>

namespace Gj {
namespace Gui {

class MuteButton final : public QPushButton {

  public:
    MuteButton(QWidget* parent, QAction* muteChannelAction, int channelIndex);
    void setMute(float val);

  private:
    int channelIndex;
    QAction* muteChannelAction;
    void setStyle();
    void mousePressEvent(QMouseEvent* event) override;
};

} // Gui
} // Gj



#endif //MUTEBUTTON_H
