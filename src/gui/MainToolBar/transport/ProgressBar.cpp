//
// Created by ns on 5/18/25.
//

#include "ProgressBar.h"


namespace Gj {
namespace Gui {

ProgressBar::ProgressBar(QWidget* parent, Audio::Mixer* mixer, const sf_count_t frameId)
  : QWidget(parent)
  , totalFrames(1)
  , frameId(frameId)
  , mixer(mixer)
  , painter(this)
  , pen(Qt::NoPen)
  {

  mixer->setUpdateProgressBarFunc(
    [this](const sf_count_t readCount, const sf_count_t newFrame) { updateProgressBar(readCount, newFrame); }
  );

  setStyle();
}

void ProgressBar::updateProgressBar(const sf_count_t readCount, const sf_count_t newFrameId) {
  totalFrames = readCount;
  frameId = newFrameId;
  update();
}

void ProgressBar::setStyle() {
  setMinimumWidth(100);
  setFixedHeight(15);
  setStyleSheet("border: 2px solid white");
}

void ProgressBar::paintEvent(QPaintEvent* event) {
  const float progress = (static_cast<float>(frameId) / (static_cast<float>(totalFrames) / 2.0f)) * width();
  painter.begin(this);
  painter.setPen(pen);
  painter.fillRect(0, 0, progress, height(), Qt::white);
  painter.end();
}

void ProgressBar::mousePressEvent(QMouseEvent* event) {
  const float x = static_cast<float>(event->position().x());
  const float percent = x / static_cast<float>(width());

  frameId = static_cast<sf_count_t>(std::floor(percent * static_cast<float>(totalFrames) / 2.0f));
  mixer->setFrameId(frameId);
  update();
}

int ProgressBar::hydrateState(const AppStatePacket &appStatePacket) {
  if (appStatePacket.playState == STOP)
    updateProgressBar(1, 0);
}

} // Gui
} // Gj