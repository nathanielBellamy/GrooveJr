//
// Created by ns on 2/28/25.
//

#ifndef MIXERWINDOW_H
#define MIXERWINDOW_H

#include "caf/actor_system.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QWidget>

#include "./Channel/MainChannelContainer.h"
#include "./Channel/EffectsChannel/EffectsChannelsContainer.h"

#include "../../AppState.h"
#include "../../audio/Mixer.h"
#include "../Color.h"

using namespace caf;

namespace Gj {
namespace Gui {

class MixerWindow final : public QWidget {
  public:
    explicit MixerWindow(QWidget *parent, actor_system& actorSystem, Audio::Mixer* mixer);
    void hydrateState(const AppStatePacket& appStatePacket);
    void setChannels();
    void setEffects();

  private:
    actor_system& actorSystem;
    Audio::Mixer* mixer;
    QAction muteChannelAction;
    QAction muteLChannelAction;
    QAction muteRChannelAction;
    QAction soloChannelAction;
    QAction soloLChannelAction;
    QAction soloRChannelAction;
    QGridLayout grid;
    QLabel title;
    MainChannelContainer mainChannelContainer;
    EffectsChannelsContainer effectsChannelsContainer;

    void setStyle();
    void setupGrid();
    void connectActions();
};

} // Gui
} // Gj

#endif //MIXERWINDOW_H
