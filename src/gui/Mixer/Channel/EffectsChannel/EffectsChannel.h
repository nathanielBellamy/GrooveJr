//
// Created by ns on 3/7/25.
//

#ifndef GUIEFFECTSCHANNEL_H
#define GUIEFFECTSCHANNEL_H

#include "caf/actor_system.hpp"

#include <QAction>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QWidget>
#include <QUrl>

#include "../../../../audio/Mixer.h"
#include "EffectsSlots.h"
#include "MuteSoloContainer.h"
#include "./Effects/EffectsContainer.h"
#include "../../../Shared/VstSelect.h"

#include "../../../../Logging.h"

using namespace caf;

namespace Gj {
namespace Gui {

class EffectsChannel final : public QWidget {

  public:
    EffectsChannel(QWidget* parent, actor_system& actorSystem, Audio::Mixer* mixer, int channelIndex);

  private:
    actor_system& actorSystem;
    Audio::Mixer* mixer;
    int channelIndex;
    EffectsContainer effectsContainer;
    QAction openEffectsContainer;
    VstSelect vstSelect;
    QUrl vstUrl;
    QAction addEffectAction;
    QGridLayout grid;
    QLabel title;
    QSlider slider;
    EffectsSlots effectsSlots;
    MuteSoloContainer muteSoloContainer;
    void setStyle();
    void setupGrid();
    void setupTitle();
    void setupSlider();
    void connectActions();
};

} // Gui
} // Gj

#endif //EFFECTSCHANNEL_H
