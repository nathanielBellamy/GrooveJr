//
// Created by ns on 3/8/25.
//

#include "PluginSlots.h"

namespace Gj {
namespace Gui {
namespace Mixer {
PluginSlots::PluginSlots(QWidget* parent,
                         actor_system& actorSystem,
                         Audio::Mixer::Core* mixer,
                         const int channelIndex,
                         QAction* replacePluginAction,
                         QAction* removePluginAction)
: QWidget(parent)
  , actorSystem(actorSystem)
  , mixer(mixer)
  , channelIndex(channelIndex)
  , grid(this)
  , replacePluginAction(replacePluginAction)
  , removePluginAction(removePluginAction) {
  setupGrid();
}

PluginSlots::~PluginSlots() {
  Logging::write(
    Info,
    "Gui::PluginSlots::~PluginSlots",
    "Destroying PluginSlots on Channel " + std::to_string(channelIndex)
  );

  reset();

  Logging::write(
    Info,
    "Gui::PluginSlots::~PluginSlots",
    "Done destroying PluginSlots on Channel " + std::to_string(channelIndex)
  );
}

void PluginSlots::hydrateState(const AppStatePacket& appState, const int newChannelIndex) {
  channelIndex = newChannelIndex;

  PluginIndex i = 0;
  for (const auto& pluginSlot: pluginSlots) {
    if (pluginSlot == nullptr) continue;
    pluginSlot->hydrateState(appState, channelIndex, i);
    ++i;
  }
}

void PluginSlots::setupGrid() {
  grid.setVerticalSpacing(4);

  int row = 0;
  for (const auto pluginSlot: pluginSlots) {
    if (pluginSlot == nullptr) continue;
    grid.addWidget(pluginSlot, row, 0, 1, 1);
    ++row;
  }
}

void PluginSlots::reset() {
  Logging::write(
    Info,
    "Gui::PluginSlots::reset",
    "Resetting PluginSlots on Channel " + std::to_string(channelIndex)
  );

  for (const auto pluginSlot: pluginSlots)
    delete pluginSlot;

  Logging::write(
    Info,
    "Gui::PluginSlots::reset",
    "Done resetting PluginSlots on channel " + std::to_string(channelIndex)
  );
}
} // Mixer
} // Gui
} // Gj
