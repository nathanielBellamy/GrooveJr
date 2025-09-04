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
#include "../../../../audio/Mixer.h"
#include "../../../../audio/Constants.h"
#include "EffectsChannel.h"
#include "AddEffectsChannelButton.h"

using namespace caf;

namespace Gj {
namespace Gui {

class EffectsChannelsContainer final : public QWidget {

  public:
    EffectsChannelsContainer(
      QWidget* parent,
      actor_system& actorSystem,
      Audio::Mixer* mixer,
      QAction* muteChannelAction,
      QAction* muteLChannelAction,
      QAction* muteRChannelAction,
      QAction* soloChannelAction,
      QAction* soloLChannelAction,
      QAction* soloRChannelAction,
      std::atomic<float>* vuPtr
    );
    void hydrateState(const AppStatePacket& appState);
    void setMute(int channelIdx, float val) const;
    void setMuteL(int channelIdx, float val) const;
    void setMuteR(int channelIdx, float val) const;
    void setSolo(int channelIdx, float val) const;
    void setSoloL(int channelIdx, float val) const;
    void setSoloR(int channelIdx, float val) const;
    void addEffectsChannel();
    void removeEffectsChannel(int channelIdx);
    void clearEffectsChannels();
    void setEffects() const;
    void setChannels();

  private:
    actor_system& actorSystem;
    Audio::Mixer* mixer;
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

} // Gui
} // Gj


#endif //EFFECTSCHANNELCONTAINER_H
