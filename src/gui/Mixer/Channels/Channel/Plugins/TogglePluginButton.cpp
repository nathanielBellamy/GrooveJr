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
  setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_DialogYesButton));
  setCursor(Qt::PointingHandCursor);
  setStyle();
}

void TogglePluginButton::hydrateState(const State::Packet& statePacket, const ChannelIndex newChannelIdx) {
  channelIndex = newChannelIdx;
  enabled = statePacket.mixerPacket.channels[channelIndex].plugins[pluginIndex].enabled;

  setStyle();
}

void TogglePluginButton::mousePressEvent(QMouseEvent* event) {
  togglePluginAction->setData(static_cast<quint64>(pluginIndex));
  togglePluginAction->activate(QAction::Trigger);
}

std::string TogglePluginButton::styleString() const {
  std::string styleString = "padding: 2px; border-radius: 5px; border: 2px solid white;";
  if (enabled) {
    styleString += " background-color: " + Color::toHex(GjC::ENABLED_GREEN) + "; ";
    styleString += " color: " + Color::toHex(GjC::DARK_400) + "; ";
  } else {
    styleString += " background-color: " + Color::toHex(GjC::DARK_300) + "; ";
  }
  return styleString;
};

void TogglePluginButton::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setMinimumSize(QSize(20, 20));
  setStyleSheet(styleString().c_str());
}
} // Mixer
} // Gui
} // Gj
