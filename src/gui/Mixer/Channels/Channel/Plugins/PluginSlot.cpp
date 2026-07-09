//
// Created by ns on 3/8/25.
//

#include "PluginSlot.h"

namespace Gj {
namespace Gui {
namespace Mixer {
PluginSlot::PluginSlot(QWidget* parent,
                       actor_system& actorSystem,
                       Audio::Mixer::Core* mixer,
                       const ChannelIndex channelIndex,
                       const PluginIndex pluginIndex,
                       const bool occupied,
                       QAction* togglePluginAction,
                       QAction* replacePluginAction,
                       QAction* removePluginAction)
: QWidget(parent)
  , actorSystem(actorSystem)
  , mixer(mixer)
  , channelIndex(channelIndex)
  , pluginIndex(pluginIndex)
  , occupied(occupied)
  , layout(this)
  , togglePluginButton(this, channelIndex, pluginIndex, occupied, togglePluginAction)
  , replacePluginButton(this, channelIndex, pluginIndex, occupied, replacePluginAction)
  , removePluginButton(this, channelIndex, pluginIndex, occupied, removePluginAction)
  , pluginName(this) {
  setContentsMargins(0, 0, 0, 0);
  pluginName.setFont({pluginName.font().family(), 10});
  pluginName.setMinimumWidth(0);
  pluginName.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  // Hide action buttons by default — revealed on hover
  replacePluginButton.hide();
  removePluginButton.hide();

  setStyle();
  setupLayout();
}

PluginSlot::~PluginSlot() {
  Logging::write(
    Info,
    "Gui::PluginSlot::~PluginSlot",
    "Destroying PluginSlot"
  );
}

void PluginSlot::hydrateState(const State::Packet& statePacket, const ChannelIndex newChannelIndex,
                              const PluginIndex newPluginIndex) {
  channelIndex = newChannelIndex;
  pluginIndex = newPluginIndex;
  pluginName.setText(statePacket.mixerPacket.channels[channelIndex].plugins[pluginIndex].name.c_str());

  togglePluginButton.hydrateState(statePacket, newChannelIndex);
}

void PluginSlot::setStyle() {
  setFixedHeight(26);
  setStyleSheet(
    QString(("background-color: " + Color::toHex(GjC::DARK_500) + "; "
             "border-bottom: 1px solid " + Color::toHex(GjC::DARK_400) + ";").data())
  );
  pluginName.setStyleSheet(
    QString(("color: " + Color::toHex(GjC::LIGHT_100) + "; "
             "background: transparent; "
             "padding-left: 4px;").data())
  );
}

void PluginSlot::setupLayout() {
  layout.setContentsMargins(2, 0, 2, 0);
  layout.setSpacing(2);

  layout.addWidget(&togglePluginButton);
  layout.addWidget(&pluginName, 1); // stretch factor 1 — takes remaining space
  layout.addWidget(&replacePluginButton);
  layout.addWidget(&removePluginButton);
}

void PluginSlot::enterEvent(QEnterEvent* event) {
  setStyleSheet(
    QString(("background-color: " + Color::toHex(GjC::DARK_300) + "; "
             "border-bottom: 1px solid " + Color::toHex(GjC::DARK_400) + ";").data())
  );
  replacePluginButton.show();
  removePluginButton.show();
  QWidget::enterEvent(event);
}

void PluginSlot::leaveEvent(QEvent* event) {
  setStyleSheet(
    QString(("background-color: " + Color::toHex(GjC::DARK_500) + "; "
             "border-bottom: 1px solid " + Color::toHex(GjC::DARK_400) + ";").data())
  );
  replacePluginButton.hide();
  removePluginButton.hide();
  QWidget::leaveEvent(event);
}
} // Mixer
} // Gui
} // Gj
