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
  for (const auto& vstWindow: vstWindows)
    vstWindow->close();

  Logging::write(
    Info,
    "Gui::PluginsContainer::~PluginsContainer",
    "Destroyed Plugins Container for Channel : " + std::to_string(channelIndex)
  );
}

void PluginsContainer::connectActions() {
  const auto selectVstWindowConnection = connect(&selectVstWindowAction, &QAction::triggered, [&]() {
    const PluginIndex pluginIndex = selectVstWindowAction.data().toULongLong();
    vstWindows.at(pluginIndex)->activateWindow();
    vstWindows.at(pluginIndex)->raise();
    activateWindow();
    raise();
  });

  const auto removePluginActionConnection = connect(removePluginAction, &QAction::triggered, [&]() {
    const PluginIndex pluginIndex = removePluginAction->data().toULongLong();
    vstWindows.at(pluginIndex)->hide();
    vstWindows.erase(vstWindows.begin() + pluginIndex);
    vstWindowSelectButtons.at(pluginIndex)->hide();
    vstWindowSelectButtons.erase(vstWindowSelectButtons.begin() + pluginIndex);
    vstWindowSelectLabels.at(pluginIndex)->hide();
    vstWindowSelectLabels.erase(vstWindowSelectLabels.begin() + pluginIndex);
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
  for (int i = 0; i < vstWindowSelectButtons.size(); i++) {
    grid.setRowMinimumHeight(i, 50);
    grid.addWidget(vstWindowSelectLabels.at(i), i, 0, 1, 1);
    grid.addWidget(vstWindowSelectButtons.at(i), i, 1, 1, 1);
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
  vstWindows.push_back(std::move(vstWindow));
  vstWindowSelectButtons.push_back(new VstWindowSelectButton(this, newPluginIndex, pluginName, &selectVstWindowAction));
  const auto label = new QLabel(this);
  label->setText((std::to_string(newPluginIndex + 1) + ".").data());
  vstWindowSelectLabels.push_back(label);

  if (isVisible()) {
    mixer->initEditorHostOnChannel(channelIndex, newPluginIndex, vstWindows.back());
    setupGrid();
    resize(width(), height() + 40);
    activateWindow();
    raise();
  }
}

void PluginsContainer::clearButtonsAndLabels() {
  for (const auto button: vstWindowSelectButtons)
    delete button;

  vstWindowSelectButtons.clear();

  for (const auto label: vstWindowSelectLabels)
    delete label;
  vstWindowSelectLabels.clear();
}

void PluginsContainer::hideEvent(QHideEvent* event) {
  mixer->terminateEditorHostsOnChannel(channelIndex);
  for (auto vstWindow: vstWindows)
    vstWindow->close();
}
} // Mixer
} // Gui
} // Gj
