//
// Created by ns on 3/25/25.
//

#include "PluginsButton.h"

namespace Gj {
namespace Gui {
namespace Mixer {
PluginsButton::PluginsButton(QWidget* parent, QAction* openPluginsContainer)
: QPushButton("&Fx", parent)
  , openPluginsContainer(openPluginsContainer) {
  setStyle();
}

void PluginsButton::setStyle() {
  setCursor(Qt::PointingHandCursor);
  setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  setStyleSheet(
    QString(
      ("width: 100%; min-width: 50px; min-height: 20px; max-height: 20px; background-color: " +
       Color::toHex(GjC::DARK_400)).data()
    )
  );
}

void PluginsButton::mousePressEvent(QMouseEvent* event) {
  openPluginsContainer->activate(QAction::Trigger);
}
} // Mixer
} // Gui
} // Gj
