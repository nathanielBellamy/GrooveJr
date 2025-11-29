//
// Created by ns on 3/16/25.
//

#include "EffectsContainer.h"

namespace Gj {
namespace Gui {

EffectsContainer::EffectsContainer(
  QWidget* parent,
  Audio::Mixer* mixer,
  const ChannelIndex channelIndex,
  QAction* addEffectAction,
  QAction* removeEffectAction
  )
  : QWidget(nullptr)
  , mixer(mixer)
  , channelIndex(channelIndex)
  , addEffectAction(addEffectAction)
  , addEffectButton(this, addEffectAction)
  , removeEffectAction(removeEffectAction)
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
  if (isVisible())
    mixer->terminateEditorHostsOnChannel(channelIndex);
  for (const auto& vstWindow : vstWindows)
    vstWindow->close();

  Logging::write(
    Info,
    "Gui::EffectsContainer::~EffectsContainer",
    "Destroyed Effects Container for Channel : " + std::to_string(channelIndex)
  );
}

void EffectsContainer::connectActions() {
  const auto selectVstWindowConnection = connect(&selectVstWindowAction, &QAction::triggered, [&]() {
    const EffectIndex effectIndex = selectVstWindowAction.data().toULongLong();
    vstWindows.at(effectIndex)->activateWindow();
    vstWindows.at(effectIndex)->raise();
    activateWindow();
    raise();
  });

  const auto removeEffectActionConnection = connect(removeEffectAction, &QAction::triggered, [&]() {
    const EffectIndex effectIndex = selectVstWindowAction.data().toULongLong();
    vstWindows.at(effectIndex)->hide();
    vstWindows.erase(vstWindows.begin() + effectIndex);
    vstWindowSelectButtons.at(effectIndex)->hide();
    vstWindowSelectButtons.erase(vstWindowSelectButtons.begin() + effectIndex);
    vstWindowSelectLabels.at(effectIndex)->hide();
    vstWindowSelectLabels.erase(vstWindowSelectLabels.begin() + effectIndex);
  });
}

void EffectsContainer::setStyle() {
  setMinimumSize(QSize(300, 200));
  setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
  setStyleSheet(
    ("background-color: " + Color::toHex(GjC::DARK_400) + ";").data()
  );
}

void EffectsContainer::setupGrid() {
  grid.setVerticalSpacing(10);

  int j = 0;
  for (int i = 0; i < vstWindowSelectButtons.size(); i++) {
    grid.setRowMinimumHeight(i, 50);
    grid.addWidget(vstWindowSelectLabels.at(i), i, 0, 1, 1);
    grid.addWidget(vstWindowSelectButtons.at(i), i, 1, 1, 1);
    j++;
  }
  grid.setRowMinimumHeight(j, 50);
  grid.addWidget(&addEffectButton, j, 0, 1, -1);

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

void EffectsContainer::addEffect(const EffectIndex newEffectIndex, const AtomicStr& pluginName) {
  auto vstWindow = std::make_shared<VstWindow>(nullptr, channelIndex, newEffectIndex, pluginName);
  vstWindows.push_back(std::move(vstWindow));
  vstWindowSelectButtons.push_back(new VstWindowSelectButton(this, newEffectIndex, pluginName, &selectVstWindowAction));
  const auto label = new QLabel(this);
  label->setText((std::to_string(newEffectIndex + 1) + ".").data());
  vstWindowSelectLabels.push_back(label);

  if (isVisible()) {
    mixer->initEditorHostOnChannel(channelIndex, newEffectIndex, vstWindows.back());
    setupGrid();
    resize(width(), height() + 40);
    activateWindow();
    raise();
  }
}

void EffectsContainer::clearButtonsAndLabels() {
  for (const auto button : vstWindowSelectButtons)
    delete button;

  vstWindowSelectButtons.clear();

  for (const auto label : vstWindowSelectLabels)
    delete label;
  vstWindowSelectLabels.clear();
}

void EffectsContainer::hideEvent(QHideEvent *event) {
  mixer->terminateEditorHostsOnChannel(channelIndex);
  for (auto vstWindow : vstWindows)
    vstWindow->close();
}

} // Gui
} // Gj