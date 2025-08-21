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
  setFixedWidth(Audio::FFT_EQ_FREQ_SIZE);
  setFixedHeight(75);
  setStyleSheet("border: 2px solid white");
}

void EqGraph::paintEvent(QPaintEvent* event) {
  const int barWidth = 1; // width() / Audio::FFT_EQ_FREQ_SIZE;
  painter.begin(this);
  painter.setPen(pen);
  for (int i = 0; i < Audio::FFT_EQ_FREQ_SIZE; i++) {
    const int barHeightL = std::min(static_cast<int>(eqBuffer[2 * i] * height() / 10), height() / 2);
    const int barHeightR = std::min(static_cast<int>(eqBuffer[2 * i + i] * height() / 10), height() / 2);
    painter.fillRect(barWidth * i, height() / 2, barWidth, barHeightL, Qt::white);
    painter.fillRect(barWidth * i, height() / 2, barWidth, -barHeightR, Qt::white);
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