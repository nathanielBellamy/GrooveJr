//
// Created by ns on 3/8/25.
//

#include "EffectSlot.h"

namespace Gj {
namespace Gui {

EffectSlot::EffectSlot(QWidget* parent, actor_system& actorSystem, Audio::Mixer* mixer, int channelIndex, int slotIndex, bool occupied)
  : QWidget(parent)
  , actorSystem(actorSystem)
  , mixer(mixer)
  , channelIndex(channelIndex)
  , slotIndex(slotIndex)
  , occupied(occupied)
  , grid(this)
  , title(this)
  , addEffectAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), tr("&Add Effect"), this)
  , addSwapButton(this, channelIndex, slotIndex, occupied, &addEffectAction)
  , vstSelect(this)
  , pluginName(this)
  {

  title.setText(QString::number(slotIndex + 1));
  title.setFont({title.font().family(), 12});

  addEffectAction.setStatusTip(tr("Add effect"));
  addAction(&addEffectAction);

  connect(&vstSelect, &QFileDialog::urlSelected, [&](const QUrl& url) {
    vstUrl = url;
  });

  connect(&addEffectAction, &QAction::triggered, [&]() {
    if (vstSelect.exec() == QDialog::Accepted) {
      std::cout <<
        "vst selected: " << vstUrl.toDisplayString().toStdString() <<
          " for channel " << this->channelIndex << std::endl <<
            " vstWindow count " << mixer->vstWindows.size() << std::endl;
      if (!mixer->addEffectToChannel(0, vstUrl.toDisplayString().toStdString().substr(7)))
        std::cout << "=== Unable to add effect to channel" << std::endl;
      // strong_actor_ptr effectsManager = actorSystem.registry().get(Gj::Act::ActorIds::EFFECTS_MANAGER);
      // const scoped_actor self{actorSystem};
      // self->anon_send(
      //     actor_cast<actor>(effectsManager),
      //     actor_cast<strong_actor_ptr>(effectsManager), // TODO
      //     this->channelIndex,
      //     vstUrl.toDisplayString().toStdString(),
      //     add_effect_a_v
      // );
    }
  });

  setStyle();
  setupGrid();
}

void EffectSlot::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setMinimumSize(QSize(100, 50));
  setStyleSheet("background-color: blue; border: 2px solid white;");
}

void EffectSlot::setupGrid() {
  grid.addWidget(&title, 0, 0, 1, 1);
  grid.addWidget(&addSwapButton, 0, 1, 1, 1);
  grid.addWidget(&pluginName, 1, 0, -1, -1);

  grid.setColumnMinimumWidth(1, 30);
  grid.setColumnStretch(1, 10);

  grid.setVerticalSpacing(0);
  grid.setHorizontalSpacing(1);
}

} // Gui
} // Gj