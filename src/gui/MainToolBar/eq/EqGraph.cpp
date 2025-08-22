//
// Created by ns on 8/20/25.
//

#include "EqGraph.h"

namespace Gj {
namespace Gui {

EqGraph::EqGraph(QWidget* parent, Audio::Mixer* mixer)
  : QWidget(parent)
  , mixer(mixer)
  , painter(this)
  , pen(Qt::NoPen)
  {

  mixer->setUpdateEqGraphFunc(
    [this](float* eqBuffer) { updateEqGraph(eqBuffer); }
  );

  setStyle();
}

void EqGraph::updateEqGraph(float* newEqBuffer) {
  eqBuffer = newEqBuffer;
  update();
}

void EqGraph::setStyle() {
  setFixedWidth(Audio::FFT_EQ_FREQ_SIZE - 2 * trim);
  setFixedHeight(75);
  setStyleSheet("border: 2px solid white");
}

void EqGraph::paintEvent(QPaintEvent* event) {
  const int barWidth = 1; // width() / Audio::FFT_EQ_FREQ_SIZE;
  const int h = height();
  painter.begin(this);
  painter.setPen(pen);
  for (int i = trim; i < Audio::FFT_EQ_FREQ_SIZE - trim; i++) {
    const int barHeightL = std::min(static_cast<int>(eqBuffer[2 * i - trim] * h / 10), h / 2);
    const int barHeightR = std::min(static_cast<int>(eqBuffer[2 * i + i - trim] * h / 10), h / 2);
    painter.fillRect(barWidth * i - trim, h / 2, barWidth, barHeightL, Qt::white);
    painter.fillRect(barWidth * i - trim, h / 2, barWidth, -barHeightR, Qt::white);
  }
  painter.end();
}

void EqGraph::mousePressEvent(QMouseEvent* event) {
  // todo
}

Result EqGraph::hydrateState(const AppStatePacket &appStatePacket) {
  if (appStatePacket.playState == STOP)
    // TODO: clear

  return OK;
}

} // Gui
} // Gj