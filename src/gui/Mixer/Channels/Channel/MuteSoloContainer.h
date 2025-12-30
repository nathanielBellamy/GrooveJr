//
// Created by ns on 3/8/25.
//

#ifndef MUTESOLOCONTAINER_H
#define MUTESOLOCONTAINER_H

#include <QAction>
#include <QGridLayout>
#include <QWidget>

#include "../../../../state/Core.h"
#include "../../../../Logging.h"

#include "../../../../audio/mixer/ChannelSettings.h"
#include "../../../../audio/mixer/Core.h"

#include "MuteButton.h"
#include "SoloButton.h"
#include "Plugins/PluginsButton.h"

namespace Gj {
namespace Gui {
namespace Mixer {
class MuteSoloContainer final : public QWidget {
public:
  MuteSoloContainer(
    QWidget* parent,
    Audio::Mixer::Core* mixer,
    ChannelIndex channelIndex,
    QAction* openPluginsContainer,
    QAction* muteChannelAction,
    QAction* muteLChannelAction,
    QAction* muteRChannelAction,
    QAction* soloChannelAction,
    QAction* soloLChannelAction,
    QAction* soloRChannelAction
  );

  ~MuteSoloContainer();

  void hydrateState(const State::Packet& appState, ChannelIndex newChannelIdx);

  void setMute(float val);

  void setMuteL(float val);

  void setMuteR(float val);

  void setSolo(float val);

  void setSoloL(float val);

  void setSoloR(float val);

private:
  Audio::Mixer::Core* mixer;
  ChannelIndex channelIndex;
  QGridLayout grid;
  MuteButton mute;
  MuteButton muteL;
  MuteButton muteR;
  SoloButton solo;
  SoloButton soloL;
  SoloButton soloR;
  PluginsButton plugins;

  void setupGrid();
};
} // Mixer
} // Gui
} // Gj


#endif //MUTESOLOCONTAINER_H
