//
// Created by ns on 4/30/25.
//

#include "RemovePluginButton.h"

namespace Gj {
namespace Gui {
namespace Mixer {
RemovePluginButton::RemovePluginButton(
  QWidget* parent,
  const ChannelIndex channelIndex,
  const PluginIndex pluginIndex,
  const bool occupied,
  QAction* action
)
: QPushButton("", parent)
  , channelIndex(channelIndex)
  , pluginIndex(pluginIndex)
  , occupied(occupied)
  , removePluginAction(action) {
  setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_TitleBarCloseButton));
  setCursor(Qt::PointingHandCursor);
  setStyle();
}

void RemovePluginButton::hydrateState(const State::Packet& appState, ChannelIndex newChannelIdx) {
  channelIndex = newChannelIdx;
}

void RemovePluginButton::mousePressEvent(QMouseEvent* event) {
  removePluginAction->setData(static_cast<quint64>(pluginIndex));
  removePluginAction->activate(QAction::Trigger);
}

void RemovePluginButton::setStyle() {
  setFixedSize(QSize(16, 16));
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setIconSize(QSize(12, 12));
  setStyleSheet("padding: 0px; border: none; background: transparent;");
}
} // Mixer
} // Gui
} // Gj
