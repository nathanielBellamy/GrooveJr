//
// Created by ns on 3/25/25.
//

#ifndef EFFECTSBUTTON_H
#define EFFECTSBUTTON_H

#include <QAction>
#include <QPushButton>

#include "../../../Color.h"

namespace Gj {
namespace Gui {

class EffectsButton final : public QPushButton {

  public:
    EffectsButton(QWidget* parent, QAction* action);

  private:
    QAction* openEffectsContainer;
    void setStyle();
    void mousePressEvent(QMouseEvent* event) override;
};

} // Gui
} // Gj

#endif //EFFECTSBUTTON_H
