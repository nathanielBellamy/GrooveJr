//
// Created by ns on 3/8/25.
//

#ifndef MAINDRYCONTAINER_H
#define MAINDRYCONTAINER_H

#include "caf/actor_system.hpp"

#include <QGridLayout>
#include <QWidget>

#include "../../../AppState.h"
#include "EffectsChannel/EffectsChannel.h"
#include "../../../audio/Mixer.h"

using namespace caf;

namespace Gj {
namespace Gui {

class MainChannelContainer final : public QWidget {

  public:
    MainChannelContainer(
      QWidget* parent,
      actor_system& actorSystem,
      Audio::Mixer* mixer,
      QAction* muteChannelAction,
      QAction* soloChannelAction
    );
    void hydrateState(const AppStatePacket& appState);
    void setMute(float val) {
      mainChannel.setMute(val);
    };
    void setSolo(float val) {
      mainChannel.setSolo(val);
    };

  private:
    actor_system& actorSystem;
    Audio::Mixer* mixer;
    QGridLayout grid;
    EffectsChannel mainChannel;
    void setStyle();
    void setupGrid();
};

} // Gui
} // Gj


#endif //MAINDRYCONTAINER_H
