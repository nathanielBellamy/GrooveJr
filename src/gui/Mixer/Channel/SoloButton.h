//
// Created by ns on 3/8/25.
//

#ifndef SOLOBUTTON_H
#define SOLOBUTTON_H

#include <QAction>
#include <QPushButton>

namespace Gj {
namespace Gui {

class SoloButton final : public QPushButton {

  public:
    SoloButton(QWidget* parent, QAction* soloChannelAction, int channelIndex);
    void setSolo(float val);

  private:
    int channelIndex;
    QAction* soloChannelAction;
    void setStyle();
    void mousePressEvent(QMouseEvent* event) override;

};

} // Gui
} // Gj


#endif //SOLOBUTTON_H
