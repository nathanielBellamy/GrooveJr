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
  , removeChannelAction(action) {
  setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_TitleBarCloseButton));
  setCursor(Qt::PointingHandCursor);
  setStyle();
}

RemoveChannelButton::~RemoveChannelButton() {
  Logging::write(
    Info,
    "Gui::Mixer::RemoveChannelButton::~RemoveChannelButton",
    "Destroying RemoveChannelButton"
  );
}


void RemoveChannelButton::hydrateState(const State::Packet& appState, const ChannelIndex newChannelIdx) {
  channelIndex = newChannelIdx;
}


void RemoveChannelButton::mousePressEvent(QMouseEvent* event) {
  removeChannelAction->setData(static_cast<quint64>(channelIndex));
  removeChannelAction->activate(QAction::Trigger);
}

void RemoveChannelButton::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setMinimumSize(QSize(20, 20));
  setStyleSheet("padding: 2px;");
}
} // Mixer
} // Gui
} // Gj
