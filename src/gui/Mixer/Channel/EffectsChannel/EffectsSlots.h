//
// Created by ns on 3/8/25.
//

#ifndef EFFECTSSLOTS_H
#define EFFECTSSLOTS_H

#include <memory>
#include <vector>

#include "caf/actor_system.hpp"

#include <QGridLayout>
#include <QWidget>

#include "EffectSlot.h"

using namespace caf;

namespace Gj {
namespace Gui {

class EffectsSlots final : public QWidget {

  public:
    EffectsSlots(QWidget* parent, actor_system& actorSystem, int channelIndex);

  private:
    actor_system& actorSystem;
    int channelIndex;
    QGridLayout grid;
    std::vector<std::unique_ptr<EffectSlot>> effectsSlots;
    void setupGrid();

};

} // Gui
} // Gj


#endif //EFFECTSSLOTS_H
