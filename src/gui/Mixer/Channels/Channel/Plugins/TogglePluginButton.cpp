//
// Created by ns on 4/5/26.
//

#include "TogglePluginButton.h"

namespace Gj {
namespace Gui {
namespace Mixer {
TogglePluginButton::TogglePluginButton(
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
  , togglePluginAction(action) {
  setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_TitleBarCloseButton));
  setCursor(Qt::PointingHandCursor);
  setStyle();
}

void TogglePluginButton::hydrateState(const State::Packet& appState, ChannelIndex newChannelIdx) {
  channelIndex = newChannelIdx;
}

void TogglePluginButton::mousePressEvent(QMouseEvent* event) {
  togglePluginAction->setData(static_cast<quint64>(pluginIndex));
  togglePluginAction->activate(QAction::Trigger);
}

void TogglePluginButton::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setMinimumSize(QSize(20, 20));
  setStyleSheet("padding: 2px;");
}
} // Mixer
} // Gui
} // Gj
