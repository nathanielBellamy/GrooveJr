//
// Created by ns on 3/16/25.
//

#include "EffectsContainer.h"

namespace Gj {
namespace Gui {

EffectsContainer::EffectsContainer(QWidget* parent, Audio::Mixer* mixer, int channelIndex)
  : QWidget(nullptr)
  , mixer(mixer)
  , channelIndex(channelIndex)
  , grid(this)
  , title(this)
  {

  if (channelIndex == 0) {
    setWindowTitle("Main Channel Effects");
  } else {
    setWindowTitle("Channel " + QString::number(channelIndex) + " Effects");
  }

  title.setText("EffectsContainer");
  title.setFont({title.font().family(), 18});

  setStyle();
  setupGrid();

  hide();
}

EffectsContainer::~EffectsContainer() {
  Logging::write(
    Info,
    "Gui::EffectsContainer::~EffectsContainer",
    "Destroying Effects Container for Channel : " + std::to_string(channelIndex)
  );
  for (const auto button : vstWindowSelectButtons) {
    delete button;
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
  });
}


void EffectsContainer::setStyle() {
  setMinimumSize(QSize(600, 600));
  setStyleSheet("background-color: aqua;");
}

void EffectsContainer::setupGrid() {
  grid.setVerticalSpacing(10);

  grid.addWidget(&title, 0, 0, 1, -1);
  grid.setRowMinimumHeight(0, 40);

  // int i = 0;
  // for (auto&& vstWindow : vstWindows) {
  //   grid.addWidget(vstWindow.get(), i, 0, 1, -1);
  //   grid.setRowMinimumHeight(i, 300);
  //   i++;
  // }

  int i = 0;
  for (const auto button : vstWindowSelectButtons) {
    grid.addWidget(button, 2, i++, 1, 1);
  }

  grid.setColumnStretch(0, 1);
  grid.setColumnStretch(1, 10);
  grid.setRowMinimumHeight(0, 20);
  grid.setRowStretch(1, 10);

  setLayout(&grid);
}

void EffectsContainer::showEvent(QShowEvent *event) {
  initVstWindows();
}

void EffectsContainer::initVstWindows() {
  for (int i = 0; i < mixer->effectsOnChannelCount(channelIndex); i++) {
    auto vstWindow = std::make_shared<VstWindow>(nullptr, channelIndex, i, mixer->getPluginName(channelIndex, i));
    vstWindows.push_back(std::move(vstWindow));
    vstWindowSelectButtons.push_back(new VstWindowSelectButton(this, i, &selectVstWindowAction));
  }
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

void EffectsContainer::hideEvent(QHideEvent *event) {
  terminateVstWindows();
}

void EffectsContainer::terminateVstWindows() {
  mixer->terminateEditorHostsOnChannel(channelIndex);
  for (auto vstWindow : vstWindows) {
    vstWindow->close();
    vstWindow.reset();
  }
  vstWindows.clear();
}

} // Gui
} // Gj