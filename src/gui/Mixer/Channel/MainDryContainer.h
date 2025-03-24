//
// Created by ns on 3/8/25.
//

#ifndef MAINDRYCONTAINER_H
#define MAINDRYCONTAINER_H

#include "caf/actor_system.hpp"

#include <QGridLayout>
#include <QWidget>

#include "DryChannel.h"
#include "EffectsChannel/EffectsChannel.h"
#include "../../../audio/Mixer.h"

using namespace caf;

namespace Gj {
namespace Gui {

class MainDryContainer final : public QWidget {

  public:
    MainDryContainer(QWidget* parent, actor_system& actorSystem, Audio::Mixer* mixer);

  private:
    actor_system& actorSystem;
    Audio::Mixer* mixer;
    QGridLayout grid;
    DryChannel dryChannel;
    EffectsChannel mainChannel;
    void setStyle();
    void setupGrid();
};

} // Gui
} // Gj


#endif //MAINDRYCONTAINER_H
