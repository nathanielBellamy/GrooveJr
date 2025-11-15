//
// Created by ns on 3/8/25.
//

#include "EffectSlot.h"

namespace Gj {
namespace Gui {

EffectSlot::EffectSlot(QWidget* parent,
                       actor_system& actorSystem,
                       Audio::Mixer* mixer,
                       const int channelIndex,
                       const int slotIndex,
                       const bool occupied,
                       QAction* replaceEffectAction,
                       QAction* removeEffectAction)
  : QWidget(parent)
  , actorSystem(actorSystem)
  , mixer(mixer)
  , channelIndex(channelIndex)
  , slotIndex(slotIndex)
  , occupied(occupied)
  , grid(this)
  , title(this)
  , replaceEffectButton(this, channelIndex, slotIndex, occupied, replaceEffectAction)
  , removeEffectButton(this, channelIndex, slotIndex, occupied, removeEffectAction)
  , pluginName(this)
  {
  title.setText(QString::number(slotIndex + 1));
  title.setFont({title.font().family(), 12});
  pluginName.setFont({pluginName.font().family(), 12});

  setStyle();
  setupGrid();
}

EffectSlot::~EffectSlot() {
  Logging::write(
    Info,
    "Gui::EffectSlot::~EffectSlot",
    "Destroying EffectSlot"
  );
}

void EffectSlot::hydrateState(const AppStatePacket& appState, const int newChannelIndex) {
  channelIndex = newChannelIndex;
  const auto name = mixer->getEffectsChannels().at(channelIndex)->getPluginAtIdx(slotIndex)->name;
  pluginName.setText(name.c_str());

  if (appState.playState == PLAY || appState.playState == FF || appState.playState == RW) {
    replaceEffectButton.setEnabled(false);
    removeEffectButton.setEnabled(false);
  } else {
    replaceEffectButton.setEnabled(true);
    removeEffectButton.setEnabled(true);
  }

  update();
}

void EffectSlot::setStyle() {
  setFixedSize(QSize(150, 70));
  setStyleSheet(
    ("border: 2px solid white; background-color: " + Color::toHex(GjC::LIGHT_200) + "; ").data()
  );
}

void EffectSlot::setupGrid() {
  grid.addWidget(&title, 0, 0, 1, 1);
  grid.addWidget(&replaceEffectButton, 0, 2, 1, 1);
  grid.addWidget(&removeEffectButton, 0, 3, 1, 1);
  grid.addWidget(&pluginName, 1, 0, -1, -1);

  grid.setColumnMinimumWidth(1, 30);
  grid.setColumnStretch(1, 10);

  grid.setVerticalSpacing(4);
  grid.setHorizontalSpacing(4);
}

} // Gui
} // Gj