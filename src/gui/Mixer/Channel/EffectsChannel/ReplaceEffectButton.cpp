//
// Created by ns on 3/8/25.
//

#include "ReplaceEffectButton.h"

namespace Gj {
namespace Gui {

ReplaceEffectButton::ReplaceEffectButton(QWidget *parent, int channelIndex, int slotIndex, bool occupied, QAction* action)
  : QPushButton("", parent)
  , channelIndex(channelIndex)
  , slotIndex(slotIndex)
  , occupied(occupied)
  , addEffectAction(action) {
  setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_BrowserReload));
  setCursor(Qt::PointingHandCursor);
}

void ReplaceEffectButton::mousePressEvent(QMouseEvent* event) {
  addEffectAction->activate(QAction::Trigger);
}

} // Gui
} // Gj