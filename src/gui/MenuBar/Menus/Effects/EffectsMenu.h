//
// Created by ns on 2/25/25.
//

#ifndef EFFECTSMENU_H
#define EFFECTSMENU_H

#include <iostream>

#include "caf/actor_from_state.hpp"
#include "caf/actor_ostream.hpp"
#include "caf/actor_registry.hpp"
#include "caf/actor_system.hpp"
#include "caf/caf_main.hpp"
#include "caf/event_based_actor.hpp"
#include "caf/scoped_actor.hpp"

#include <QAction>
#include <QFileDialog>
#include <QMenu>
#include <QWidget>

#include "./VstSelect.h"

namespace Gj {
namespace Gui {

using namespace caf;

class EffectsMenu final : public QMenu {
  private:
    actor_system& actorSystem;
    QAction* addEffectAction;
    VstSelect* vstSelect;
    QUrl vstUrl;

  public:
    EffectsMenu(actor_system& actorSystem, QWidget* parent);
    ~EffectsMenu() override;
};

} // Gui
} // Gj



#endif //EFFECTSMENU_H
