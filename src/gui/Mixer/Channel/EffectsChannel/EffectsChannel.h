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
#include <QScrollArea>
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
#include "../../../Color.h"

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
      QAction* muteLChannelAction,
      QAction* muteRChannelAction,
      QAction* soloChannelAction,
      QAction* soloLChannelAction,
      QAction* soloRChannelAction
    );
    ~EffectsChannel() override;
    void hydrateState(const AppStatePacket& appStatePacket, int newChannelIndex);
    void updateShowRemoveEffectsChannelButton(bool val);
    void setMute(float val);
    void setMuteL(float val);
    void setMuteR(float val);
    void setSolo(float val);
    void setSoloL(float val);
    void setSoloR(float val);
    void setEffects();
    void addEffect(int effectIndex);
    void reset();

  private:
    actor_system& actorSystem;
    strong_actor_ptr appStateManagerPtr;
    Audio::Mixer* mixer;
    int channelIndex;
    QAction* removeEffectsChannelAction;
    RemoveEffectsChannelButton removeEffectsChannelButton;
    QAction addEffectAction;
    AddEffectButton addEffectButton;
    EffectsContainer effectsContainer;
    QAction openEffectsContainer;
    VstSelect vstSelect;
    QUrl vstUrl;
    QAction replaceEffectAction;
    QAction removeEffectAction;
    QGridLayout grid;
    QLabel title;
    QSlider gainSlider;
    QLabel  gainLabel;
    QSlider gainLSlider;
    QLabel  gainLLabel;
    QSlider gainRSlider;
    QLabel  gainRLabel;
    QSlider panSlider;
    QLabel  panLabel;
    QSlider panLSlider;
    QLabel  panLLabel;
    QSlider panRSlider;
    QLabel  panRLabel;
    EffectsSlots effectsSlots;
    QScrollArea effectsSlotsScrollArea;
    MuteSoloContainer muteSoloContainer;
    void setStyle();
    void setupGrid();
    void setupTitle();
    void setupGainSlider();
    void setupGainLSlider();
    void setupGainRSlider();
    void setupPanSlider();
    void setupPanLSlider();
    void setupPanRSlider();
    void connectActions();
    void setupEffectsSlotsScrollArea();
};

} // Gui
} // Gj

#endif //GUIEFFECTSCHANNEL_H
