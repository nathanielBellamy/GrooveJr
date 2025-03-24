//
// Created by ns on 3/7/25.
//

#ifndef EFFECTSCHANNELCONTAINER_H
#define EFFECTSCHANNELCONTAINER_H

#include "caf/actor_system.hpp"

#include <memory>
#include <vector>

#include <QGridLayout>
#include <QWidget>

#include "../../../../audio/Mixer.h"
#include "EffectsChannel.h"

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
    void setStyle();
    void setupGrid();

};

} // Gui
} // Gj


#endif //EFFECTSCHANNELCONTAINER_H
