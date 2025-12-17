//
// Created by ns on 5/3/25.
//

#include "RemoveChannelButton.h"

namespace Gj {
namespace Gui {
namespace Mixer {
RemoveChannelButton::RemoveChannelButton(QWidget* parent, ChannelIndex channelIndex, QAction* action)
: QPushButton("", parent)
  , channelIndex(channelIndex)
  , removeEffectsChannelAction(action) {
  setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_TitleBarCloseButton));
  setCursor(Qt::PointingHandCursor);
  setStyle();
}

RemoveChannelButton::~RemoveChannelButton() {
  Logging::write(
    Info,
    "Gui::RemoveEffectsChannelButton::~RemoveEffectsChannelButton",
    "Destroying RemoveEffectsChannelButton"
  );
}


void RemoveChannelButton::hydrateState(const AppStatePacket& appState, const ChannelIndex newChannelIdx) {
  channelIndex = newChannelIdx;
}


void RemoveChannelButton::mousePressEvent(QMouseEvent* event) {
  removeEffectsChannelAction->setData(static_cast<quint64>(channelIndex));
  removeEffectsChannelAction->activate(QAction::Trigger);
}

void RemoveChannelButton::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setMinimumSize(QSize(20, 20));
  setStyleSheet("padding: 2px;");
}
} // Mixer
} // Gui
} // Gj
