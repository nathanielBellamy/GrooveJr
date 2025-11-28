//
// Created by ns on 5/3/25.
//

#include "RemoveEffectsChannelButton.h"

namespace Gj {
namespace Gui {

RemoveEffectsChannelButton::RemoveEffectsChannelButton(QWidget *parent, ChannelIndex channelIndex, QAction* action)
  : QPushButton("", parent)
  , channelIndex(channelIndex)
  , removeEffectsChannelAction(action) {
  setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_TitleBarCloseButton));
  setCursor(Qt::PointingHandCursor);
  setStyle();
}

RemoveEffectsChannelButton::~RemoveEffectsChannelButton() {
  Logging::write(
    Info,
    "Gui::RemoveEffectsChannelButton::~RemoveEffectsChannelButton",
    "Destroying RemoveEffectsChannelButton"
  );
}


void RemoveEffectsChannelButton::hydrateState(const AppStatePacket &appState, const ChannelIndex newChannelIdx) {
  channelIndex = newChannelIdx;
}


void RemoveEffectsChannelButton::mousePressEvent(QMouseEvent* event) {
  removeEffectsChannelAction->setData(static_cast<quint64>(channelIndex));
  removeEffectsChannelAction->activate(QAction::Trigger);
}

void RemoveEffectsChannelButton::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setMinimumSize(QSize(20, 20));
  setStyleSheet("padding: 2px;");
}

} // Gui
} // Gj
