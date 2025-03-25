//
// Created by ns on 3/25/25.
//

#ifndef EFFECTSBUTTON_H
#define EFFECTSBUTTON_H

#include <QPushButton>

namespace Gj {
namespace Gui {

class EffectsButton final : public QPushButton {

  public:
    EffectsButton(QWidget* parent);

  private:
    void setStyle();
};

} // Gui
} // Gj

#endif //EFFECTSBUTTON_H
