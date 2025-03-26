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

  auto vstWindow1 = std::make_unique<VstWindow>(this);
  mixer->vstWindows.push_back(std::move(vstWindow1));
  std::cout << "VstWindow added to Mixer" << std::endl;
  // TODO
  // - here for testing
  // - init editorhost here avoid Qt threading error
  // - rendering vstwindow within qt
  // - todo: style
  // auto foo = mixer->addEffectToChannel(0, "/Library/Audio/Plug-Ins/VST3/ValhallaSupermassive.vst3");

  setStyle();
  setupGrid();

  hide();
}

void EffectsContainer::setStyle() {
  setMinimumSize(QSize(600, 600));
  setStyleSheet("background-color: aqua;");
}

void EffectsContainer::setupGrid() {
  grid.setVerticalSpacing(1);

  grid.addWidget(&title, 0, 0, 1, -1);
  grid.addWidget(mixer->vstWindows.front().get(), 1, 0, -1, -1);

  grid.setColumnStretch(0, 1);
  grid.setColumnStretch(1, 10);
  grid.setRowMinimumHeight(0, 20);
  grid.setRowStretch(1, 10);

  setLayout(&grid);
}

void EffectsContainer::initVstWindows() {
  for (int i = 0; i < mixer->effectsOnChannelCount(channelIndex); i++) {
    auto vstWindow = std::make_unique<VstWindow>(this);
    vstWindows.push_back(std::move(vstWindow));
  }
  mixer->initEditorHostsOnChannel(channelIndex, vstWindows);
}



} // Gui
} // Gj