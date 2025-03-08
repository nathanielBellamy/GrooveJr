//
// Created by ns on 3/8/25.
//

#ifndef MUTEBUTTON_H
#define MUTEBUTTON_H

#include <QPushButton>

namespace Gj {
namespace Gui {


class MuteButton final : public QPushButton {

  public:
    MuteButton(QWidget* parent);

  private:
    void setStyle();
};

} // Gui
} // Gj



#endif //MUTEBUTTON_H
