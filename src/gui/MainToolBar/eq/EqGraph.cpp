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

  // TODO:
//  mixer->setUpdateEqGraphFunc(
//    [this](float* eqBuffer) { updateEqGraph(newFrames, newFrame); }
//  );

  setStyle();
}

void EqGraph::updateEqGraph(float* newEqBuffer) {
  eqBuffer = newEqBuffer;
  update();
}

void EqGraph::setStyle() {
  setMinimumWidth(100);
  setFixedHeight(75);
  setStyleSheet("border: 2px solid white");
}

void EqGraph::paintEvent(QPaintEvent* event) {
  painter.begin(this);
  painter.setPen(pen);
  for (int i = 0; i < Audio::FFT_EQ_FREQ_SIZE; i++) {
    painter.fillRect(0, height() / 2, width(), eqBuffer[2 * i], Qt::white);
    painter.fillRect(0, height() / 2, width(), -eqBuffer[2 * i + 1], Qt::white);
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