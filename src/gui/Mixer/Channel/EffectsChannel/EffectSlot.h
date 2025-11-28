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

#include "../../../../AppState.h"
#include "RemoveEffectButton.h"
#include "ReplaceEffectButton.h"
#include "../../../Shared/VstSelect.h"
#include "../../../../actors/ActorIds.h"
#include "../../../../messaging/atoms.h"
#include "../../../../audio/Mixer.h"
#include "../../../Color.h"

using namespace caf;

namespace Gj {
namespace Gui {

class EffectSlot final : public QWidget {

  public:
    EffectSlot(QWidget* parent,
               actor_system& actorSystem,
               Audio::Mixer* mixer,
               ChannelIndex channelIndex,
               EffectIndex effectIndex,
               bool occupied,
               QAction* replaceEffectAction,
               QAction* removeEffectAction);
    ~EffectSlot() override;
    void hydrateState(const AppStatePacket& appState, ChannelIndex newChannelIndex);

  private:
    actor_system& actorSystem;
    Audio::Mixer* mixer;
    ChannelIndex channelIndex;
    EffectIndex effectIndex;
    bool occupied;
    QGridLayout grid;
    QLabel title;
    ReplaceEffectButton replaceEffectButton;
    RemoveEffectButton removeEffectButton;
    QUrl vstUrl;
    QLabel pluginName;
    void setStyle();
    void setupGrid();
};

} // Gui
} // Gj

#endif //EFFECTSLOT_H
