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

#include "../../../../audio/ChannelSettings.h"
#include "../../../../audio/Mixer.h"

#include "../MuteButton.h"
#include "../SoloButton.h"
#include "EffectsButton.h"

namespace Gj {
namespace Gui {
class MuteSoloContainer final : public QWidget {
public:
  MuteSoloContainer(
    QWidget* parent,
    Audio::Mixer* mixer,
    ChannelIndex channelIndex,
    QAction* openEffectsContainer,
    QAction* muteChannelAction,
    QAction* muteLChannelAction,
    QAction* muteRChannelAction,
    QAction* soloChannelAction,
    QAction* soloLChannelAction,
    QAction* soloRChannelAction
  );

  ~MuteSoloContainer();

  void hydrateState(const AppStatePacket& appState, ChannelIndex newChannelIdx);

  void setMute(float val);

  void setMuteL(float val);

  void setMuteR(float val);

  void setSolo(float val);

  void setSoloL(float val);

  void setSoloR(float val);

private:
  Audio::Mixer* mixer;
  ChannelIndex channelIndex;
  QGridLayout grid;
  MuteButton mute;
  MuteButton muteL;
  MuteButton muteR;
  SoloButton solo;
  SoloButton soloL;
  SoloButton soloR;
  EffectsButton effects;

  void setupGrid();
};
} // Gui
} // Gj


#endif //MUTESOLOCONTAINER_H
