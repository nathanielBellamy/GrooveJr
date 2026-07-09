//
// Created by ns on 3/8/25.
//

#include "ReplacePluginButton.h"

namespace Gj {
namespace Gui {
namespace Mixer {
ReplacePluginButton::ReplacePluginButton(
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
  , replacePluginAction(action) {
  setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_BrowserReload));
  setCursor(Qt::PointingHandCursor);
  setStyle();
}

ReplacePluginButton::~ReplacePluginButton() {
  Logging::write(
    Info,
    "Gui::ReplacePluginButton::~ReplacePluginButton",
    "Destroying ReplacePluginButton."
  );
}

void ReplacePluginButton::hydrateState(const State::Packet& appState, int newChannelIdx) {
  channelIndex = newChannelIdx;
}

void ReplacePluginButton::mousePressEvent(QMouseEvent* event) {
  replacePluginAction->setData(static_cast<quint64>(pluginIndex));
  replacePluginAction->activate(QAction::Trigger);
}

void ReplacePluginButton::setStyle() {
  setFixedSize(QSize(16, 16));
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setIconSize(QSize(12, 12));
  setStyleSheet("padding: 0px; border: none; background: transparent;");
}
} // Mixer
} // Gui
} // Gj
