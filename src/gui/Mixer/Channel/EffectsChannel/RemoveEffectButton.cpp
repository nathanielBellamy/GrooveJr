//
// Created by ns on 4/30/25.
//

#include "RemoveEffectButton.h"

namespace Gj {
namespace Gui {

RemoveEffectButton::RemoveEffectButton(QWidget *parent, int channelIndex, int slotIndex, bool occupied, QAction* action)
  : QPushButton("", parent)
  , channelIndex(channelIndex)
  , slotIndex(slotIndex)
  , occupied(occupied)
  , removeEffectAction(action) {
  setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_TitleBarCloseButton));
  setCursor(Qt::PointingHandCursor);
  setStyle();
}

void RemoveEffectButton::mousePressEvent(QMouseEvent* event) {
  removeEffectAction->setData(QVariant(slotIndex));
  removeEffectAction->activate(QAction::Trigger);
}

void RemoveEffectButton::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setMinimumSize(QSize(20, 20));
  setStyleSheet("padding: 2px;");
}

} // Gui
} // Gj
