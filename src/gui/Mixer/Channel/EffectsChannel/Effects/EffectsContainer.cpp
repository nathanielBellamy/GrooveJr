//
// Created by ns on 3/16/25.
//

#include "EffectsContainer.h"

namespace Gj {
namespace Gui {

EffectsContainer::EffectsContainer(QWidget* parent, Audio::Mixer* mixer, int channelIndex, QAction* addEffectAction)
  : QWidget(nullptr)
  , mixer(mixer)
  , channelIndex(channelIndex)
  , addEffectAction(addEffectAction)
  , addEffectButton(this, addEffectAction)
  , grid(this)
  {

  if (channelIndex == 0) {
    setWindowTitle("Main Channel Effects");
  } else {
    setWindowTitle("Channel " + QString::number(channelIndex) + " Effects");
  }

  setStyle();
  setupGrid();
  connectActions();

  hide();
}

EffectsContainer::~EffectsContainer() {
  Logging::write(
    Info,
    "Gui::EffectsContainer::~EffectsContainer",
    "Destroying Effects Container for Channel : " + std::to_string(channelIndex)
  );

  clearButtonsAndLabels();
  mixer->terminateEditorHostsOnChannel(channelIndex);
  for (auto vstWindow : vstWindows) {
    vstWindow->close();
  }

  Logging::write(
    Info,
    "Gui::EffectsContainer::~EffectsContainer",
    "Destroyed Effects Container for Channel : " + std::to_string(channelIndex)
  );
}

void EffectsContainer::connectActions() {
  auto selectVstWindowConnection = connect(&selectVstWindowAction, &QAction::triggered, [&]() {
    const int effectIndex = selectVstWindowAction.data().toInt();
    vstWindows.at(effectIndex)->activateWindow();
    vstWindows.at(effectIndex)->raise();
    activateWindow();
    raise();
  });
}

void EffectsContainer::setStyle() {
  setMinimumSize(QSize(300, 200));
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
  setStyleSheet(
    ("background-color: " + Color::toHex(GjC::DARK_400) + ";").data()
  );
}

void EffectsContainer::setupGrid() {
  int j = 0;
  for (int i = 0; i < vstWindowSelectButtons.size(); i++) {
    grid.addWidget(vstWindowSelectLabels.at(i), i, 0, 1, 1);
    grid.addWidget(vstWindowSelectButtons.at(i), i, 1, 1, 1);
    j++;
  }
  grid.addWidget(&addEffectButton, j, 0, 1, -1);

  grid.setVerticalSpacing(4);
  setLayout(&grid);
}

void EffectsContainer::showEvent(QShowEvent *event) {
  Logging::write(
    Info,
    "EffectsContainer::initVstWindows()",
    "Created VstWindows for channel: " + std::to_string(channelIndex)
  );

  setupGrid();
  mixer->initEditorHostsOnChannel(channelIndex, vstWindows);
  for (auto&& vstWindow : vstWindows) {
    vstWindow->activateWindow();
    vstWindow->raise();
    vstWindow->show();
  }
  Logging::write(
    Info,
    "EffectsContainer::initVstWindows()",
    "Editorhosts initialized for channel: " + std::to_string(channelIndex)
  );
}

void EffectsContainer::addEffect(const int newEffectIndex, const std::string pluginName) {
  auto vstWindow = std::make_shared<VstWindow>(nullptr, channelIndex, newEffectIndex, pluginName);
  vstWindows.push_back(std::move(vstWindow));
  vstWindowSelectButtons.push_back(new VstWindowSelectButton(this, newEffectIndex, pluginName, &selectVstWindowAction));
  const auto label = new QLabel(this);
  label->setText((std::to_string(newEffectIndex + 1) + ".").data());
  vstWindowSelectLabels.push_back(label);

  if (isVisible()) {
    mixer->initEditorHostOnChannel(channelIndex, newEffectIndex, vstWindows.back());
    setupGrid();
  }
}

void EffectsContainer::clearButtonsAndLabels() {
  for (const auto button : vstWindowSelectButtons) {
    delete button;
  }
  vstWindowSelectButtons.clear();

  for (const auto label : vstWindowSelectLabels) {
    delete label;
  }
  vstWindowSelectLabels.clear();
}

void EffectsContainer::hideEvent(QHideEvent *event) {
  mixer->terminateEditorHostsOnChannel(channelIndex);
  for (auto vstWindow : vstWindows) {
    vstWindow->close();
  }
}

} // Gui
} // Gj