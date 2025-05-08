//
// Created by ns on 3/8/25.
//

#ifndef MUTESOLOCONTAINER_H
#define MUTESOLOCONTAINER_H

#include <QAction>
#include <QGridLayout>
#include <QWidget>

#include "../../../../AppState.h"
#include "../../../../Logging.h"
#include "../MuteButton.h"
#include "../SoloButton.h"
#include "EffectsButton.h"

namespace Gj {
namespace Gui {

class MuteSoloContainer final : public QWidget {
  public:
    MuteSoloContainer(
      QWidget* parent,
      QAction* openEffectsContainer,
      QAction* muteChannelAction,
      QAction* soloChannelAction,
      int channelIndex
    );
    ~MuteSoloContainer();
    void hydrateState(const AppStatePacket& appState, int newChannelIdx);

  private:
    int channelIndex;
    QGridLayout grid;
    MuteButton mute;
    SoloButton solo;
    EffectsButton effects;
    void setupGrid();

};

} // Gui
} // Gj



#endif //MUTESOLOCONTAINER_H
