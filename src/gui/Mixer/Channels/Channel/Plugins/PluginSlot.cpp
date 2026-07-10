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
  pluginName.setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

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
  fullPluginName = QString(statePacket.mixerPacket.channels[channelIndex].plugins[pluginIndex].name.c_str());
  updateElidedName();

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
  updateElidedName();
  QWidget::enterEvent(event);
}

void PluginSlot::leaveEvent(QEvent* event) {
  setStyleSheet(
    QString(("background-color: " + Color::toHex(GjC::DARK_500) + "; "
             "border-bottom: 1px solid " + Color::toHex(GjC::DARK_400) + ";").data())
  );
  replacePluginButton.hide();
  removePluginButton.hide();
  updateElidedName();
  QWidget::leaveEvent(event);
}

void PluginSlot::resizeEvent(QResizeEvent* event) {
  QWidget::resizeEvent(event);
  updateElidedName();
}

void PluginSlot::updateElidedName() {
  if (fullPluginName.isEmpty()) return;

  // Calculate available width for the name label:
  // slot width minus layout margins, toggle button, spacings, and label padding
  int availableWidth = width() - 4 // layout margins (2+2)
                       - 14 // toggle button
                       - 2 // spacing after toggle
                       - 4; // label padding-left

  if (replacePluginButton.isVisible()) {
    availableWidth -= 16 + 2; // replace button + spacing
    availableWidth -= 16 + 2; // remove button + spacing
  }

  if (availableWidth <= 0) availableWidth = 40;

  QFontMetrics fm(pluginName.font());
  pluginName.setText(fm.elidedText(fullPluginName, Qt::ElideRight, availableWidth));
}
} // Mixer
} // Gui
} // Gj
