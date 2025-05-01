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

#include "../../../../AppState.h"
#include "AddEffectSlotButton.h"
#include "EffectSlot.h"
#include "../../../../audio/Mixer.h"

using namespace caf;

namespace Gj {
namespace Gui {

class EffectsSlots final : public QWidget {

  public:
    EffectsSlots(
      QWidget* parent,
      actor_system& actorSystem,
      Audio::Mixer* mixer,
      int channelIndex,
      QAction* addEffectAction,
      QAction* replaceEffectAction,
      QAction* removeEffectAction
    );
    void addEffectSlot();
    void removeEffectSlot();
    void hydrateState(const AppStatePacket& appState) const;

  private:
    actor_system& actorSystem;
    Audio::Mixer* mixer;
    int channelIndex;
    QGridLayout grid;
    std::vector<std::unique_ptr<EffectSlot>> effectsSlots;
    AddEffectSlotButton addEffectSlotButton;
    QAction* replaceEffectAction;
    QAction* removeEffectAction;
    void setupGrid();

};

} // Gui
} // Gj


#endif //EFFECTSSLOTS_H
