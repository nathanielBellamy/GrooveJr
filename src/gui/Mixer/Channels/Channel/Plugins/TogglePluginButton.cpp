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
  std::string styleString = "border-radius: 5px; border: none;";
  if (enabled) {
    styleString += " background-color: " + Color::toHex(GjC::ENABLED_GREEN) + ";";
  } else {
    styleString += " background-color: " + Color::toHex(GjC::DARK_300) + ";";
  }
  return styleString;
};

void TogglePluginButton::setStyle() {
  setFixedSize(QSize(14, 14));
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setStyleSheet(styleString().c_str());
}
} // Mixer
} // Gui
} // Gj
