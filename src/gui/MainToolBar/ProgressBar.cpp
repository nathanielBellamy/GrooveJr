//
// Created by ns on 5/18/25.
//

#include "ProgressBar.h"


namespace Gj {
namespace Gui {

ProgressBar::ProgressBar(QWidget* parent, Audio::Mixer* mixer, sf_count_t frame)
  : QWidget(parent)
  , totalFrames(1)
  , frame(frame)
  , mixer(mixer)
  , painter(this)
  , pen(Qt::NoPen)
  {

  mixer->setUpdateProgressBarFunc(
    [this](sf_count_t readCount, sf_count_t newFrame) { updateProgressBar(readCount, newFrame); }
  );

  setStyle();
}

void ProgressBar::updateProgressBar(sf_count_t readCount, sf_count_t newFrame) {
  totalFrames = readCount;
  frame = newFrame;
  update();
}

void ProgressBar::setStyle() {
  setMinimumWidth(100);
  setFixedHeight(15);
  setStyleSheet("border: 2px solid white");
}

void ProgressBar::paintEvent(QPaintEvent* event) {
  float progress = (static_cast<float>(frame) / static_cast<float>(totalFrames)) * width();
  painter.begin(this);
  painter.setPen(pen);
  painter.fillRect(0, 0, progress, height(), Qt::white);
  painter.end();
}


} // Gui
} // Gj