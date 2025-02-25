//
// Created by ns on 2/25/25.
//

#include "EffectsMenu.h"

namespace Gj {
namespace Gui {

EffectsMenu::EffectsMenu(actor_system& actorSystem, QWidget* parent)
    : QMenu("&Effects", parent)
    , actorSystem(actorSystem)
    , addEffectAction(new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), tr("&Add Effect"), this))
    {

  addEffectAction->setStatusTip(tr("Add effect to audio"));
  connect(addEffectAction, &QAction::triggered, [&]() {
    // TODO
    std::cout << " select VST3 effect " << std::endl;
  });
  addAction(addEffectAction);

}


} // Gui
} // Gj
