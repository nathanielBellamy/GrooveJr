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
                         const ChannelIndex channelIndex,
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

void PluginSlots::hydrateState(const State::Packet& statePacket, const ChannelIndex newChannelIndex) {
  channelIndex = newChannelIndex;

  for (const auto pluginPacket: statePacket.mixerPacket.channels[channelIndex].plugins) {
    if (!pluginPacket.hasValue) {
      delete pluginSlots[pluginPacket.pluginIndex];
      pluginSlots[pluginPacket.pluginIndex] = nullptr;
    } else {
      if (pluginSlots[pluginPacket.pluginIndex] == nullptr) {
        pluginSlots[pluginPacket.pluginIndex] =
            new PluginSlot(
              this,
              actorSystem,
              mixer,
              channelIndex,
              pluginPacket.pluginIndex,
              true,
              replacePluginAction,
              removePluginAction
            );

        pluginSlots[pluginPacket.pluginIndex]->hydrateState(statePacket, channelIndex, pluginPacket.pluginIndex);
        grid.addWidget(pluginSlots[pluginPacket.pluginIndex], 0, 0, 1, 1);
      }
    }
  }

  PluginIndex i = 0;
  for (PluginIndex plugIdx = 0; plugIdx < Audio::MAX_PLUGINS_PER_CHANNEL; ++plugIdx) {
    if (pluginSlots[plugIdx] == nullptr) continue;
    pluginSlots[plugIdx]->hydrateState(statePacket, channelIndex, i);
    ++i;
  }

  setupGrid();
}

void PluginSlots::setupGrid() {
  grid.setVerticalSpacing(4);

  int row = 0;
  for (PluginIndex plugIdx = 0; plugIdx < Audio::MAX_PLUGINS_PER_CHANNEL; ++plugIdx) {
    if (pluginSlots[plugIdx] == nullptr) continue;
    grid.addWidget(pluginSlots[plugIdx], row, 0, 1, 1);
    ++row;
  }

  setLayout(&grid);
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
