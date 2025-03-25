//
// Created by ns on 2/28/25.
//

#ifndef MIXERWINDOW_H
#define MIXERWINDOW_H

#include "caf/actor_system.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QWidget>

#include "./Channel/MainDryContainer.h"
#include "./Channel/EffectsChannel/EffectsChannelsContainer.h"

#include "../../audio/Mixer.h"

using namespace caf;

namespace Gj {
namespace Gui {

class MixerWindow final : public QWidget {
  public:
    explicit MixerWindow(QWidget *parent, actor_system& actorSystem, Audio::Mixer* mixer);

  private:
    actor_system& actorSystem;
    Audio::Mixer* mixer;
    QGridLayout grid;
    QLabel title;
    MainDryContainer mainDryContainer;
    EffectsChannelsContainer effectsChannelsContainer;

    void setStyle();
    void setupGrid();
};

} // Gui
} // Gj

#endif //MIXERWINDOW_H
