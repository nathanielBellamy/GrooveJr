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

void PluginSlots::hydrateState(const State::Packet& statePacket, const int newChannelIndex) {
  std::cout << "PluginSlots::hydrateState" << std::endl;
  channelIndex = newChannelIndex;

  PluginIndex i = 0;
  for (const auto pluginPacket: statePacket.mixerPacket.channels[channelIndex].plugins) {
    std::cout << "ps::hs - pluginPacket " << pluginPacket.pluginIndex << std::endl;
    if (!pluginPacket.hasValue) {
      std::cout << "ps::hs - pluginPacket no val" << std::endl;
      delete pluginSlots[pluginPacket.pluginIndex];
      pluginSlots[pluginPacket.pluginIndex] = nullptr;
    } else {
      std::cout << "ps::hs - pluginPacket has val - name " << pluginPacket.name.c_str() << std::endl;
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
      }
    }
  }

  for (const auto& pluginSlot: pluginSlots) {
    if (pluginSlot == nullptr) continue;
    pluginSlot->hydrateState(statePacket, channelIndex, i);
    ++i;
  }

  setupGrid();
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
