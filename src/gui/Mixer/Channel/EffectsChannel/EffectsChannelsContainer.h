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
#include <QWidget>

#include "../../../../Logging.h"
#include "../../../../audio/Mixer.h"
#include "EffectsChannel.h"
#include "AddEffectsChannelButton.h"

using namespace caf;

namespace Gj {
namespace Gui {

class EffectsChannelsContainer final : public QWidget {

  public:
    EffectsChannelsContainer(QWidget* parent, actor_system& actorSystem, Audio::Mixer* mixer);

  private:
    actor_system& actorSystem;
    Audio::Mixer* mixer;
    QGridLayout grid;
    QWidget spacer;
    std::vector<std::unique_ptr<EffectsChannel>> channels;
    QAction addEffectsChannelAction;
    AddEffectsChannelButton addEffectsChannelButton;
    void addEffectsChannel();
    void connectActions();
    void setStyle();
    void setupGrid();

};

} // Gui
} // Gj


#endif //EFFECTSCHANNELCONTAINER_H
