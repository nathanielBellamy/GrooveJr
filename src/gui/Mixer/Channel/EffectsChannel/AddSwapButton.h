//
// Created by ns on 3/8/25.
//

#ifndef ADDSWAPBUTTON_H
#define ADDSWAPBUTTON_H

#include <QPushButton>

namespace Gj {
namespace Gui {

class AddSwapButton final : public QPushButton {

  public:
    AddSwapButton(QWidget* parent, bool occupied);

  private:
    bool occupied;

};

} // Gui
} // Gj



#endif //ADDSWAPBUTTON_H
