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
    "Gui::ReplaceEffectButton::~ReplaceEffectButton",
    "Destroying ReplaceEffectButton."
  );
}


void ReplacePluginButton::hydrateState(const AppStatePacket& appState, int newChannelIdx) {
  channelIndex = newChannelIdx;
}

void ReplacePluginButton::mousePressEvent(QMouseEvent* event) {
  replacePluginAction->setData(static_cast<quint64>(pluginIndex));
  replacePluginAction->activate(QAction::Trigger);
}

void ReplacePluginButton::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setMinimumSize(QSize(20, 20));
  setStyleSheet("padding: 2px;");
}
} // Mixer
} // Gui
} // Gj
