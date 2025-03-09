//
// Created by ns on 3/8/25.
//

#ifndef EFFECTSSLOTS_H
#define EFFECTSSLOTS_H

#include <memory>
#include <vector>

#include <QGridLayout>
#include <QWidget>

#include "EffectSlot.h"

namespace Gj {
namespace Gui {

class EffectsSlots final : public QWidget {

  public:
    EffectsSlots(QWidget* parent, int channelIndex);

  private:
    int channelIndex;
    QGridLayout grid;
    std::vector<std::unique_ptr<EffectSlot>> effectsSlots;
    void setupGrid();

};

} // Gui
} // Gj


#endif //EFFECTSSLOTS_H
