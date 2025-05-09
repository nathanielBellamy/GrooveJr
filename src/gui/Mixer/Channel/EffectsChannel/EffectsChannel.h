//
// Created by ns on 3/7/25.
//

#ifndef GUIEFFECTSCHANNEL_H
#define GUIEFFECTSCHANNEL_H

#include "caf/actor_system.hpp"
#include "caf/scoped_actor.hpp"
#include "../../../../messaging/atoms.h"

#include <QAction>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QWidget>
#include <QUrl>

#include "../../../../AppState.h"

#include "../../../../audio/Mixer.h"
#include "EffectsSlots.h"
#include "MuteSoloContainer.h"
#include "RemoveEffectsChannelButton.h"
#include "./Effects/EffectsContainer.h"
#include "../../../Shared/VstSelect.h"

#include "../../../../Logging.h"

using namespace caf;

namespace Gj {
namespace Gui {

class EffectsChannel final : public QWidget {

  public:
    EffectsChannel(
      QWidget* parent,
      actor_system& actorSystem,
      Audio::Mixer* mixer,
      int channelIndex,
      QAction* removeEffectsChannelAction,
      QAction* muteChannelAction,
      QAction* soloChannelAction
    );
    ~EffectsChannel();
    void hydrateState(const AppStatePacket& appStatePacket, int newChannelIndex);
    void updateShowRemoveEffectsChannelButton(bool val);
    void setMute(float val);
    void setSolo(float val);

  private:
    actor_system& actorSystem;
    strong_actor_ptr appStateManagerPtr;
    Audio::Mixer* mixer;
    int channelIndex;
    QAction* removeEffectsChannelAction;
    RemoveEffectsChannelButton removeEffectsChannelButton;
    EffectsContainer effectsContainer;
    QAction openEffectsContainer;
    VstSelect vstSelect;
    QUrl vstUrl;
    QAction addEffectAction;
    QAction replaceEffectAction;
    QAction removeEffectAction;
    QGridLayout grid;
    QLabel title;
    QSlider gainSlider;
    QSlider panSlider;
    EffectsSlots effectsSlots;
    MuteSoloContainer muteSoloContainer;
    void setStyle();
    void setupGrid();
    void setupTitle();
    void setupGainSlider();
    void setupPanSlider();
    void connectActions();
};

} // Gui
} // Gj

#endif //EFFECTSCHANNEL_H
