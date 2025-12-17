//
// Created by ns on 3/25/25.
//

#ifndef EFFECTSBUTTON_H
#define EFFECTSBUTTON_H

#include <QAction>
#include <QPushButton>

#include "../../../../Color.h"

namespace Gj {
namespace Gui {
namespace Mixer {
class PluginsButton final : public QPushButton {
public:
  PluginsButton(QWidget* parent, QAction* openEffectsContainer);

private:
  QAction* openEffectsContainer;

  void setStyle();

  void mousePressEvent(QMouseEvent* event) override;
};
} // Mixer
} // Gui
} // Gj

#endif //EFFECTSBUTTON_H
