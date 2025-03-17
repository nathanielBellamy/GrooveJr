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
#include "./Effects/EffectsContainer.h"

using namespace caf;

namespace Gj {
namespace Gui {

class MixerWindow final : public QWidget {
  public:
    explicit MixerWindow(QWidget *parent, actor_system& actorSystem);

  private:
    actor_system& actorSystem;
    QGridLayout grid;
    QLabel title;
    MainDryContainer mainDryContainer;
    EffectsChannelsContainer effectsChannelsContainer;
    EffectsContainer effectsContainer;

    void setStyle();
    void setupGrid();
};

} // Gui
} // Gj

#endif //MIXERWINDOW_H
