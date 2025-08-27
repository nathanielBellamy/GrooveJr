//
// Created by ns on 5/18/25.
//

#include "ProgressBar.h"


namespace Gj {
namespace Gui {

ProgressBar::ProgressBar(QWidget* parent, Audio::Mixer* mixer, const sf_count_t frameId)
  : QWidget(parent)
  , frames(1)
  , frameId(frameId)
  , mixer(mixer)
  // , painter(this)
  // , pen(Qt::NoPen)
  {

  mixer->setUpdateProgressBarFunc(
    [this](const sf_count_t newFrames, const sf_count_t newFrame) { updateProgressBar(newFrames, newFrame); }
  );

  setStyle();
}

void ProgressBar::updateProgressBar(const sf_count_t newFrames, const sf_count_t newFrameId) {
  frames = newFrames;
  frameId = newFrameId;
  update();
}

void ProgressBar::setStyle() {
  setMinimumWidth(100);
  setFixedHeight(15);
  setStyleSheet("border: 2px solid white");
}

void ProgressBar::paintEvent(QPaintEvent* event) {
  // const int progress = static_cast<int>(
  //   std::floor((static_cast<float>(frameId) / static_cast<float>(frames)) * static_cast<float>(width()))
  // );
  // QPainter painter(this);
  // const QPen pen(Qt::NoPen);
  // painter.begin(this);
  // painter.setPen(pen);
  // // painter.fillRect(0, 0, progress, height(), Qt::white);
  // painter.end();
}

void ProgressBar::mousePressEvent(QMouseEvent* event) {
  const float x = static_cast<float>(event->position().x());
  const float percent = x / static_cast<float>(width());

  frameId = static_cast<sf_count_t>(std::floor(percent * static_cast<float>(frames)));
  Audio::Mixer::setFrameId(frameId);
  update();
}

Result ProgressBar::hydrateState(const AppStatePacket &appStatePacket) {
  if (appStatePacket.playState == STOP)
    updateProgressBar(1, 0);

  return OK;
}

} // Gui
} // Gj