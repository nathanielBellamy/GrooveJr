//
// Created by ns on 3/8/25.
//

#ifndef SOLOBUTTON_H
#define SOLOBUTTON_H

#include <QPushButton>

namespace Gj {
namespace Gui {

class SoloButton final : public QPushButton {

  public:
    SoloButton(QWidget* parent);

  private:
    void setStyle();

};

} // Gui
} // Gj


#endif //SOLOBUTTON_H
