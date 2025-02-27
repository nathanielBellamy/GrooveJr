//
// Created by ns on 2/25/25.
//

#include "EffectsMenu.h"

namespace Gj {
namespace Gui {

EffectsMenu::~EffectsMenu() {
  delete addEffectAction;
  delete vstSelect;
  delete this;
}

EffectsMenu::EffectsMenu(actor_system& actorSystem, QWidget* parent)
    : QMenu("&Effects", parent)
    , actorSystem(actorSystem)
    , addEffectAction(new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), tr("&Add Effect"), this))
    , vstSelect(new VstSelect(this))
    {

  addEffectAction->setStatusTip(tr("Add effect to audio"));
  addAction(addEffectAction);

  connect(vstSelect, &QFileDialog::urlSelected, [&](const QUrl& url) {
    vstUrl = url;
  });

  connect(addEffectAction, &QAction::triggered, [&]() {
    if (vstSelect->exec() == QDialog::Accepted) {
      std::cout << "vst selected: " << vstUrl.toDisplayString().toStdString() << std::endl;
    }
  });
}


} // Gui
} // Gj
