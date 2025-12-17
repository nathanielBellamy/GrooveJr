//
// Created by ns on 3/8/25.
//

#ifndef EFFECTSLOT_H
#define EFFECTSLOT_H

#include <iostream>

#include "caf/actor_system.hpp"
#include "caf/actor_registry.hpp"
#include "caf/scoped_actor.hpp"

#include <QAction>
#include <QGridLayout>
#include <QLabel>
#include <QWidget>

#include "../../../../../AppState.h"
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
             PluginIndex effectIndex,
             bool occupied,
             QAction* replaceEffectAction,
             QAction* removeEffectAction);

  ~PluginSlot() override;

  void hydrateState(const AppStatePacket& appState, ChannelIndex newChannelIndex);

private:
  actor_system& actorSystem;
  Audio::Mixer::Core* mixer;
  ChannelIndex channelIndex;
  PluginIndex pluginIndex;
  bool occupied;
  QGridLayout grid;
  QLabel title;
  ReplacePluginButton replaceEffectButton;
  RemovePluginButton removeEffectButton;
  QUrl vstUrl;
  QLabel pluginName;

  void setStyle();

  void setupGrid();
};
} // Mixer
} // Gui
} // Gj

#endif //EFFECTSLOT_H
