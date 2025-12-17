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

#include "../../../../../Logging.h"
#include "../../../../../AppState.h"
#include "AddPluginButton.h"
#include "PluginSlot.h"
#include "../../../../../audio/mixer/Core.h"

using namespace caf;

namespace Gj {
namespace Gui {
namespace Mixer {
class PluginSlots final : public QWidget {
public:
  PluginSlots(
    QWidget* parent,
    actor_system& actorSystem,
    Audio::Mixer::Core* mixer,
    int channelIndex,
    QAction* replaceEffectAction,
    QAction* removeEffectAction
  );

  ~PluginSlots() override;

  void addEffectSlot();

  void removeEffectSlot();

  void hydrateState(const AppStatePacket& appState, int newChannelIndex);

  void reset();

private:
  actor_system& actorSystem;
  Audio::Mixer::Core* mixer;
  int channelIndex;
  QGridLayout grid;
  std::vector<std::unique_ptr<PluginSlot> > effectsSlots{};
  QAction* replaceEffectAction;
  QAction* removeEffectAction;

  void setupGrid();
};
} // Mixer
} // Gui
} // Gj


#endif //EFFECTSSLOTS_H
