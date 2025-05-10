//
// Created by ns on 3/7/25.
//

#ifndef EFFECTSCHANNELCONTAINER_H
#define EFFECTSCHANNELCONTAINER_H

#include "caf/actor_system.hpp"
#include "caf/scoped_actor.hpp"
#include "../../../../messaging/atoms.h"

#include <memory>
#include <vector>

#include <QAction>
#include <QGridLayout>
#include <QScrollArea>
#include <QWidget>

#include "../../../../Logging.h"
#include "../../../../AppState.h"
#include "../../../../audio/Mixer.h"
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
      QAction* soloChannelAction
    );
    void hydrateState(const AppStatePacket& appState);
    void setMute(int channelIdx, float val) const;
    void setSolo(int channelIdx, float val) const;

  private:
    actor_system& actorSystem;
    Audio::Mixer* mixer;
    QGridLayout grid;
    QWidget spacer;
    QWidget channelsWidget;
    QWidget channelsWidgetInner;
    QScrollArea channelsScrollArea;
    QGridLayout channelsGrid;
    QGridLayout channelsGridInner;
    std::vector<EffectsChannel*> channels;
    QAction addEffectsChannelAction;
    QAction removeEffectsChannelAction;
    AddEffectsChannelButton addEffectsChannelButton;
    QAction* muteChannelAction;
    QAction* soloChannelAction;
    void addEffectsChannel();
    void removeEffectsChannel(int channelIdx);
    void connectActions();
    void setStyle();
    void setupGrid();

};

} // Gui
} // Gj


#endif //EFFECTSCHANNELCONTAINER_H
