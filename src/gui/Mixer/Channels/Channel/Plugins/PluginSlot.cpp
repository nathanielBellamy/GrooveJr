//
// Created by ns on 3/8/25.
//

#include "PluginSlot.h"

namespace Gj {
namespace Gui {
namespace Mixer {
PluginSlot::PluginSlot(QWidget* parent,
                       actor_system& actorSystem,
                       Audio::Mixer::Core* mixer,
                       const ChannelIndex channelIndex,
                       const PluginIndex pluginIndex,
                       const bool occupied,
                       QAction* replacePluginAction,
                       QAction* removePluginAction)
: QWidget(parent)
  , actorSystem(actorSystem)
  , mixer(mixer)
  , channelIndex(channelIndex)
  , pluginIndex(pluginIndex)
  , occupied(occupied)
  , grid(this)
  , title(this)
  , replacePluginButton(this, channelIndex, pluginIndex, occupied, replacePluginAction)
  , removePluginButton(this, channelIndex, pluginIndex, occupied, removePluginAction)
  , pluginName(this) {
  title.setText(QString::number(pluginIndex + 1));
  title.setFont({title.font().family(), 12});
  pluginName.setFont({pluginName.font().family(), 12});

  setStyle();
  setupGrid();
}

PluginSlot::~PluginSlot() {
  Logging::write(
    Info,
    "Gui::PluginSlot::~PluginSlot",
    "Destroying PluginSlot"
  );
}

void PluginSlot::hydrateState(const State::Packet& statePacket, const ChannelIndex newChannelIndex,
                              const PluginIndex newPluginIndex) {
  channelIndex = newChannelIndex;
  pluginIndex = newPluginIndex;
  pluginName.setText(statePacket.mixerPacket.channels[channelIndex].plugins[pluginIndex].name.c_str());
}

void PluginSlot::setStyle() {
  setFixedSize(QSize(150, 70));
  setStyleSheet(
    ("border: 2px solid white; background-color: " + Color::toHex(GjC::DARK_200) + "; ").data()
  );
}

void PluginSlot::setupGrid() {
  grid.addWidget(&title, 0, 0, 1, 1);
  grid.addWidget(&replacePluginButton, 0, 2, 1, 1);
  grid.addWidget(&removePluginButton, 0, 3, 1, 1);
  grid.addWidget(&pluginName, 1, 0, -1, -1);

  grid.setColumnMinimumWidth(1, 30);
  grid.setColumnStretch(1, 10);

  grid.setVerticalSpacing(4);
  grid.setHorizontalSpacing(4);
}
} // Mixer
} // Gui
} // Gj
