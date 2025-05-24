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

#include "../../../../Logging.h"
#include "../../../../AppState.h"
#include "AddEffectButton.h"
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
      QAction* replaceEffectAction,
      QAction* removeEffectAction
    );
    ~EffectsSlots() override;
    void addEffectSlot();
    void removeEffectSlot();
    void hydrateState(const AppStatePacket& appState, int newChannelIndex);

  private:
    actor_system& actorSystem;
    Audio::Mixer* mixer;
    int channelIndex;
    QGridLayout grid;
    std::vector<std::unique_ptr<EffectSlot>> effectsSlots;
    QAction* replaceEffectAction;
    QAction* removeEffectAction;
    void setupGrid();

};

} // Gui
} // Gj


#endif //EFFECTSSLOTS_H
