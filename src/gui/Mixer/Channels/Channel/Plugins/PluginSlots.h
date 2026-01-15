//
// Created by ns on 3/8/25.
//

#ifndef GJGUIMIXERPLUGINSLOTS_H
#define GJGUIMIXERPLUGINSLOTS_H

#include <memory>
#include <vector>

#include "caf/actor_system.hpp"

#include <QGridLayout>
#include <QWidget>

#include "../../../../../Logging.h"
#include "../../../../../state/Core.h"
#include "AddPluginButton.h"
#include "PluginSlot.h"
#include "../../../../../audio/mixer/Core.h"

using namespace caf;

namespace Gj {
namespace Gui {
namespace Mixer {
class PluginSlots final : public QWidget {
public:
  PluginSlots(
    QWidget* parent,
    actor_system& actorSystem,
    Audio::Mixer::Core* mixer,
    ChannelIndex channelIndex,
    QAction* replacePluginAction,
    QAction* removePluginAction
  );

  ~PluginSlots() override;

  Result hydrateState(const State::Packet& statePacket, ChannelIndex newChannelIndex);

  Result reset();

private:
  actor_system& actorSystem;
  Audio::Mixer::Core* mixer;
  ChannelIndex channelIndex;
  QGridLayout grid;
  PluginSlot* pluginSlots[Audio::MAX_PLUGINS_PER_CHANNEL]{};
  QAction* replacePluginAction;
  QAction* removePluginAction;

  void setupGrid();
};
} // Mixer
} // Gui
} // Gj


#endif //GJGUIMIXERPLUGINSLOTS_H
