//
// Created by ns on 4/30/25.
//

#include "RemoveEffectButton.h"

namespace Gj {
namespace Gui {

RemoveEffectButton::RemoveEffectButton(
  QWidget* parent,
  const ChannelIndex channelIndex,
  const PluginIndex effectIndex,
  const bool occupied,
  QAction* action
  )
  : QPushButton("", parent)
  , channelIndex(channelIndex)
  , effectIndex(effectIndex)
  , occupied(occupied)
  , removeEffectAction(action) {
  setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_TitleBarCloseButton));
  setCursor(Qt::PointingHandCursor);
  setStyle();
}

void RemoveEffectButton::hydrateState(const AppStatePacket &appState, ChannelIndex newChannelIdx) {
  channelIndex = newChannelIdx;
}

void RemoveEffectButton::mousePressEvent(QMouseEvent* event) {
  removeEffectAction->setData(static_cast<quint64>(effectIndex));
  removeEffectAction->activate(QAction::Trigger);
}

void RemoveEffectButton::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setMinimumSize(QSize(20, 20));
  setStyleSheet("padding: 2px;");
}

} // Gui
} // Gj
