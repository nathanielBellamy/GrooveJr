//
// Created by ns on 3/8/25.
//

#ifndef GJGUIMIXERPLUGINSLOT_H
#define GJGUIMIXERPLUGINSLOT_H

#include <iostream>

#include "caf/actor_system.hpp"
#include "caf/actor_registry.hpp"
#include "caf/scoped_actor.hpp"

#include <QAction>
#include <QGridLayout>
#include <QLabel>
#include <QWidget>

#include "../../../../../state/AppState.h"
#include "RemovePluginButton.h"
#include "ReplacePluginButton.h"
#include "../../../../Shared/VstSelect.h"
#include "../../../../../actors/ActorIds.h"
#include "../../../../../messaging/atoms.h"
#include "../../../../../audio/mixer/Core.h"
#include "../../../../Color.h"

using namespace caf;

namespace Gj {
namespace Gui {
namespace Mixer {
class PluginSlot final : public QWidget {
public:
  PluginSlot(QWidget* parent,
             actor_system& actorSystem,
             Audio::Mixer::Core* mixer,
             ChannelIndex channelIndex,
             PluginIndex pluginIndex,
             bool occupied,
             QAction* replacePluginAction,
             QAction* removePluginAction);

  ~PluginSlot() override;

  void hydrateState(const AppStatePacket& appState, ChannelIndex newChannelIndex, PluginIndex newPluginIndex);

private:
  actor_system& actorSystem;
  Audio::Mixer::Core* mixer;
  ChannelIndex channelIndex;
  PluginIndex pluginIndex;
  bool occupied;
  QGridLayout grid;
  QLabel title;
  ReplacePluginButton replacePluginButton;
  RemovePluginButton removePluginButton;
  QUrl vstUrl;
  QLabel pluginName;

  void setStyle();

  void setupGrid();
};
} // Mixer
} // Gui
} // Gj

#endif //GJGUIMIXERPLUGINSLOT_H
