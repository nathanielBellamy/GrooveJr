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

  pluginSlots.clear();

  Logging::write(
    Info,
    "Gui::PluginSlots::~PluginSlots",
    "Done destroying PluginSlots on Channel " + std::to_string(channelIndex)
  );
}

void PluginSlots::hydrateState(const AppStatePacket& appState, const int newChannelIndex) {
  channelIndex = newChannelIndex;

  for (const auto& pluginSlot: pluginSlots)
    pluginSlot->hydrateState(appState, channelIndex);
}

void PluginSlots::addSlot() {
  auto slot = std::make_unique<PluginSlot>(
    this,
    actorSystem,
    mixer,
    channelIndex,
    pluginSlots.size(),
    false,
    replacePluginAction,
    removePluginAction
  );
  pluginSlots.push_back(std::move(slot));
  setupGrid();
  update();
}

void PluginSlots::removeSlot() {
  pluginSlots.pop_back();
  setupGrid();
  update();
}

void PluginSlots::setupGrid() {
  grid.setVerticalSpacing(4);

  int row = 0;
  for (auto& pluginSlot: pluginSlots) {
    grid.addWidget(pluginSlot.get(), row, 0, 1, 1);
    row++;
  }
}

void PluginSlots::reset() {
  Logging::write(
    Info,
    "Gui::PluginSlots::reset",
    "Resetting PluginSlots on Channel " + std::to_string(channelIndex)
  );

  pluginSlots.clear();

  Logging::write(
    Info,
    "Gui::PluginSlots::reset",
    "Done resetting PluginSlots on channel " + std::to_string(channelIndex)
  );
}
} // Mixer
} // Gui
} // Gj
