//
// Created by ns on 3/7/25.
//

#ifndef GUIEFFECTSCHANNEL_H
#define GUIEFFECTSCHANNEL_H

#include "caf/actor_system.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QWidget>

#include "EffectsSlots.h"
#include "MuteSoloContainer.h"

using namespace caf;

namespace Gj {
namespace Gui {

class EffectsChannel final : public QWidget {

  public:
    EffectsChannel(QWidget* parent, actor_system& actorSystem, int channelIndex);

  private:
    actor_system& actorSystem;
    int channelIndex;
    QGridLayout grid;
    QLabel title;
    QSlider slider;
    EffectsSlots effectsSlots;
    MuteSoloContainer muteSoloContainer;
    void setStyle();
    void setupGrid();
};


} // Gui
} // Gj

#endif //EFFECTSCHANNEL_H
