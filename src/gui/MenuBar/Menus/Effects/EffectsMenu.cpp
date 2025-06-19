//
// Created by ns on 2/25/25.
//

#include "EffectsMenu.h"

namespace Gj {
namespace Gui {

EffectsMenu::EffectsMenu(actor_system& actorSystem, QWidget* parent)
    : QMenu("&Effects", parent)
    , actorSystem(actorSystem)
    , addEffectAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), tr("&Add Effect"), this)
    , vstSelect(this)
    {

  addEffectAction.setStatusTip(tr("Add effect to audio"));
  addAction(&addEffectAction);

  connect(&vstSelect, &QFileDialog::urlSelected, [&](const QUrl& url) {
    vstUrl = url;
  });

  connect(&addEffectAction, &QAction::triggered, [&]() {
    if (vstSelect.exec() == QDialog::Accepted) {
      Logging::write(
        Info,
        "Gui::EffectsMenu::addEffectAction",
        "Selected vst: " + vstUrl.toDisplayString().toStdString()
      );
    }
  });
}


} // Gui
} // Gj
