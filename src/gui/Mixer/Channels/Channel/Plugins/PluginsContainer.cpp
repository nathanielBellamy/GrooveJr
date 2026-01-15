//
// Created by ns on 3/16/25.
//

#include "PluginsContainer.h"

namespace Gj {
namespace Gui {
namespace Mixer {
PluginsContainer::PluginsContainer(
  QWidget* parent,
  Audio::Mixer::Core* mixer,
  const ChannelIndex channelIndex,
  QAction* addPluginAction,
  QAction* removePluginAction
)
: QWidget(nullptr)
  , mixer(mixer)
  , channelIndex(channelIndex)
  , addPluginAction(addPluginAction)
  , addPluginButton(this, addPluginAction)
  , removePluginAction(removePluginAction)
  , grid(this) {
  if (channelIndex == 0) {
    setWindowTitle("Main Channel Plugins");
  } else {
    setWindowTitle("Channel " + QString::number(channelIndex) + " Plugins");
  }

  setStyle();
  setupGrid();
  connectActions();

  hide();
}

PluginsContainer::~PluginsContainer() {
  Logging::write(
    Info,
    "Gui::PluginsContainer::~PluginsContainer",
    "Destroying Plugins Container for Channel : " + std::to_string(channelIndex)
  );

  clearButtonsAndLabels();
  if (isVisible())
    mixer->terminateEditorHostsOnChannel(channelIndex);

  for (PluginIndex i = 0; i < Audio::MAX_PLUGINS_PER_CHANNEL; ++i) {
    if (hasPluginAt(i))
      vstWindows[i]->close();
  }

  Logging::write(
    Info,
    "Gui::PluginsContainer::~PluginsContainer",
    "Destroyed Plugins Container for Channel : " + std::to_string(channelIndex)
  );
}

Result PluginsContainer::hydrateState(const State::Packet& statePacket, const ChannelIndex newChannelIndex) {
  channelIndex = newChannelIndex;

  for (const auto pluginPacket: statePacket.mixerPacket.channels[channelIndex].plugins) {
    if (!pluginPacket.hasValue) {
      if (vstWindows[pluginPacket.pluginIndex] != nullptr)
        vstWindows[pluginPacket.pluginIndex]->hide();
      vstWindows[pluginPacket.pluginIndex] = nullptr;
    } else if (vstWindows[pluginPacket.pluginIndex] == nullptr) {
      addPlugin(pluginPacket.pluginIndex, pluginPacket.name);
    }
  }

  setupGrid();

  return OK;
}

void PluginsContainer::connectActions() {
  const auto selectVstWindowConnection = connect(&selectVstWindowAction, &QAction::triggered, [&]() {
    const PluginIndex pluginIndex = selectVstWindowAction.data().toULongLong();

    if (!hasPluginAt(pluginIndex))
      return;

    vstWindows[pluginIndex]->activateWindow();
    vstWindows[pluginIndex]->raise();
    activateWindow();
    raise();
  });

  const auto removePluginActionConnection = connect(removePluginAction, &QAction::triggered, [&]() {
    const PluginIndex pluginIndex = removePluginAction->data().toULongLong();
    if (!hasPluginAt(pluginIndex))
      return;

    vstWindows[pluginIndex]->hide();
    vstWindows[pluginIndex] = nullptr;
    vstWindowSelectButtons[pluginIndex]->hide();
    vstWindowSelectButtons[pluginIndex] = nullptr;
    vstWindowSelectLabels[pluginIndex]->hide();
    vstWindowSelectLabels[pluginIndex] = nullptr;
  });
}

void PluginsContainer::setStyle() {
  setMinimumSize(QSize(300, 200));
  setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
  setStyleSheet(
    ("background-color: " + Color::toHex(GjC::DARK_400) + ";").data()
  );
}

void PluginsContainer::setupGrid() {
  grid.setVerticalSpacing(10);

  int j = 0;
  for (PluginIndex i = 0; i < Audio::MAX_PLUGINS_PER_CHANNEL; i++) {
    if (!hasPluginAt(i))
      continue;

    grid.setRowMinimumHeight(i, 50);
    grid.addWidget(vstWindowSelectLabels[i].get(), i, 0, 1, 1);
    grid.addWidget(vstWindowSelectButtons[i].get(), i, 1, 1, 1);
    j++;
  }
  grid.setRowMinimumHeight(j, 50);
  grid.addWidget(&addPluginButton, j, 0, 1, -1);

  setLayout(&grid);
}

void PluginsContainer::showEvent(QShowEvent* event) {
  Logging::write(
    Info,
    "PluginsContainer::initVstWindows()",
    "Created VstWindows for channel: " + std::to_string(channelIndex)
  );

  setupGrid();
  if (mixer->initEditorHostsOnChannel(channelIndex, vstWindows) != OK)
    Logging::write(
      Warning,
      "Gui::PluginsContainer::showEvent",
      "An Error or Warning Occurred while initializing EditorHosts"
    );

  for (auto&& vstWindow: vstWindows) {
    if (vstWindow == nullptr) continue;
    vstWindow->activateWindow();
    vstWindow->raise();
    vstWindow->show();
  }
  Logging::write(
    Info,
    "PluginsContainer::initVstWindows()",
    "Editorhosts initialized for channel: " + std::to_string(channelIndex)
  );
}

void PluginsContainer::addPlugin(const PluginIndex newPluginIndex, const AtomicStr& pluginName) {
  auto vstWindow = std::make_shared<VstWindow>(nullptr, channelIndex, newPluginIndex, pluginName);
  vstWindows[newPluginIndex] = std::move(vstWindow);
  vstWindowSelectButtons[newPluginIndex] = std::make_unique<VstWindowSelectButton>(
    this, newPluginIndex, pluginName, &selectVstWindowAction
  );

  vstWindowSelectLabels[newPluginIndex] = std::make_unique<QLabel>(this);
  vstWindowSelectLabels[newPluginIndex]->setText((std::to_string(newPluginIndex + 1) + ".").data());

  if (isVisible()) {
    mixer->initEditorHostOnChannel(channelIndex, newPluginIndex, vstWindows[newPluginIndex]);
    setupGrid();
    resize(width(), height() + 40);
    activateWindow();
    raise();
  }
}

void PluginsContainer::clearButtonsAndLabels() {
  for (PluginIndex i; i < Audio::MAX_PLUGINS_PER_CHANNEL; ++i) {
    vstWindowSelectButtons[i] = nullptr;
    vstWindowSelectLabels[i] = nullptr;
  }
}

void PluginsContainer::hideEvent(QHideEvent* event) {
  mixer->terminateEditorHostsOnChannel(channelIndex);
  for (auto vstWindow: vstWindows) {
    if (vstWindow == nullptr) continue;
    vstWindow->close();
  }
}
} // Mixer
} // Gui
} // Gj
