//
// Created by ns on 3/7/25.
//

#ifndef EFFECTSCHANNELCONTAINER_H
#define EFFECTSCHANNELCONTAINER_H

#include "caf/actor_system.hpp"
#include "caf/scoped_actor.hpp"
#include "../../../../messaging/atoms.h"

#include <algorithm>
#include <atomic>
#include <memory>
#include <vector>
#include <iterator>
#include <thread>

#include <QAction>
#include <QGridLayout>
#include <QScrollArea>
#include <QWidget>

#include "../../../../Logging.h"
#include "../../../../AppState.h"
#include "../../../../enums/Result.h"

#include "../../../Color.h"
#include "../../../../audio/mixer/Core.h"
#include "../../../../audio/Constants.h"
#include "EffectsChannel.h"
#include "AddEffectsChannelButton.h"

using namespace caf;

namespace Gj {
namespace Gui {
namespace Mixer {
class EffectsChannelsContainer final : public QWidget {
public:
  EffectsChannelsContainer(
    QWidget* parent,
    actor_system& actorSystem,
    Audio::Mixer::Core* mixer,
    QAction* muteChannelAction,
    QAction* muteLChannelAction,
    QAction* muteRChannelAction,
    QAction* soloChannelAction,
    QAction* soloLChannelAction,
    QAction* soloRChannelAction,
    std::atomic<float>* vuPtr
  );

  void hydrateState(const AppStatePacket& appState);

  void setMute(ChannelIndex channelIdx, float val) const;

  void setMuteL(ChannelIndex channelIdx, float val) const;

  void setMuteR(ChannelIndex channelIdx, float val) const;

  void setSolo(ChannelIndex channelIdx, float val) const;

  void setSoloL(ChannelIndex channelIdx, float val) const;

  void setSoloR(ChannelIndex channelIdx, float val) const;

  void addEffectsChannel();

  void removeEffectsChannel(ChannelIndex channelIdx);

  void clearEffectsChannels();

  void setEffects() const;

  void setChannels();

private:
  actor_system& actorSystem;
  Audio::Mixer::Core* mixer;
  QGridLayout grid;
  QWidget spacer;
  QWidget channelsWidget;
  QScrollArea channelsScrollArea;
  QGridLayout channelsGrid;
  std::vector<EffectsChannel*> channels;
  QAction addEffectsChannelAction;
  QAction removeEffectsChannelAction;
  AddEffectsChannelButton addEffectsChannelButton;
  QAction* muteChannelAction;
  QAction* muteLChannelAction;
  QAction* muteRChannelAction;
  QAction* soloChannelAction;
  QAction* soloLChannelAction;
  QAction* soloRChannelAction;
  std::atomic<float>* vuPtr;

  void connectActions();

  void setStyle();

  void setupGrid();

  void setupChannelsScrollArea();
};
} // Mixer
} // Gui
} // Gj


#endif //EFFECTSCHANNELCONTAINER_H
