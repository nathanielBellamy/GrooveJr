//
// Created by ns on 4/29/25.
//

#include "AddPluginButton.h"

namespace Gj {
namespace Gui {
namespace Mixer {
AddPluginButton::AddPluginButton(QWidget* parent, QAction* addPluginAction)
: QPushButton("&+", parent)
  , addPluginAction(addPluginAction) {
  setStyle();
}

void AddPluginButton::mousePressEvent(QMouseEvent* event) {
  addPluginAction->activate(QAction::Trigger);
}

void AddPluginButton::setStyle() {
  setCursor(Qt::PointingHandCursor);
  setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  setStyleSheet(
    QString(
      ("width: 100%; min-width: 50px; min-height: 20px; max-height: 20px; background-color: " +
       Color::toHex(GjC::DARK_400)).data()
    )
  );
}
} // Mixer
} // Gui
} // Gj
