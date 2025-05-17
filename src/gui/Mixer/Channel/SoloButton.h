//
// Created by ns on 3/8/25.
//

#ifndef SOLOBUTTON_H
#define SOLOBUTTON_H

#include <QAction>
#include <QPushButton>

#include "../../Color.h"

namespace Gj {
namespace Gui {

class SoloButton final : public QPushButton {

  public:
    SoloButton(QWidget* parent, QAction* soloChannelAction, int channelIndex);
    void setSolo(float val);

  private:
    int channelIndex;
    QAction* soloChannelAction;
    std::string styleString(float soloVal) const;
    void setStyle();
    void mousePressEvent(QMouseEvent* event) override;

};

} // Gui
} // Gj


#endif //SOLOBUTTON_H
