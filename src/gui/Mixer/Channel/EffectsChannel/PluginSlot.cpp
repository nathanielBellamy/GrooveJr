//
// Created by ns on 3/8/25.
//

#include "PluginSlot.h"

namespace Gj {
namespace Gui {
PluginSlot::PluginSlot(QWidget* parent,
                       actor_system& actorSystem,
                       Audio::Mixer* mixer,
                       const ChannelIndex channelIndex,
                       const PluginIndex effectIndex,
                       const bool occupied,
                       QAction* replaceEffectAction,
                       QAction* removeEffectAction)
: QWidget(parent)
  , actorSystem(actorSystem)
  , mixer(mixer)
  , channelIndex(channelIndex)
  , pluginIndex(effectIndex)
  , occupied(occupied)
  , grid(this)
  , title(this)
  , replaceEffectButton(this, channelIndex, effectIndex, occupied, replaceEffectAction)
  , removeEffectButton(this, channelIndex, effectIndex, occupied, removeEffectAction)
  , pluginName(this) {
  title.setText(QString::number(effectIndex + 1));
  title.setFont({title.font().family(), 12});
  pluginName.setFont({pluginName.font().family(), 12});

  setStyle();
  setupGrid();
}

PluginSlot::~PluginSlot() {
  Logging::write(
    Info,
    "Gui::EffectSlot::~EffectSlot",
    "Destroying EffectSlot"
  );
}

void PluginSlot::hydrateState(const AppStatePacket& appState, const ChannelIndex newChannelIndex) {
  channelIndex = newChannelIndex;
  const auto plugin = mixer->getEffectsChannels().at(channelIndex)->getPluginAtIdx(pluginIndex);
  if (!plugin) return;
  pluginName.setText(plugin.value()->name.c_str());

  if (appState.playState == PLAY || appState.playState == FF || appState.playState == RW) {
    replaceEffectButton.setEnabled(false);
    removeEffectButton.setEnabled(false);
  } else {
    replaceEffectButton.setEnabled(true);
    removeEffectButton.setEnabled(true);
  }

  update();
}

void PluginSlot::setStyle() {
  setFixedSize(QSize(150, 70));
  setStyleSheet(
    ("border: 2px solid white; background-color: " + Color::toHex(GjC::LIGHT_200) + "; ").data()
  );
}

void PluginSlot::setupGrid() {
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
