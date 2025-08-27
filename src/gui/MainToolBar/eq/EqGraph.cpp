//
// Created by ns on 8/20/25.
//

#include "EqGraph.h"

namespace Gj {
namespace Gui {

EqGraph::EqGraph(QWidget* parent, Audio::Mixer* mixer)
  : QWidget(parent)
  , mixer(mixer)
  , eqRingBuffer(nullptr)
  {

  mixer->setSetEqRingBufferFunc(
    [this](jack_ringbuffer_t* eqRingBuffer) { setEqRingBuffer(eqRingBuffer); }
  );
//  mixer->setEqAnimationStartFunc(
//    [this]() { animationStart(); }
//  );
//  mixer->setEqAnimationStopFunc(
//    [this]() { animationStop(); }
//  );

  setStyle();

  animationStart();
}

void EqGraph::setEqRingBuffer(jack_ringbuffer_t* newEqRingBuffer) {
  eqRingBuffer = newEqRingBuffer;
}

void EqGraph::animationStart() {
  animationTimer.setInterval(16); // ~60Hz
  connect(&animationTimer, &QTimer::timeout, this, &EqGraph::animationLoop);
  animationTimer.start();
}

void EqGraph::animationStop() {
  animationTimer.stop();
}

// TODO: move animation loop to its own QThread
void EqGraph::animationLoop() {
  if (eqRingBuffer == nullptr)
    return;

  if (jack_ringbuffer_read_space(eqRingBuffer) > Audio::FFT_EQ_RING_BUFFER_SIZE - 2) {
    jack_ringbuffer_read(
      eqRingBuffer,
      reinterpret_cast<char*>(eqBuffer),
      Audio::FFT_EQ_RING_BUFFER_SIZE
    );

    repaint();
  }
}

void EqGraph::setStyle() {
  setFixedWidth(Audio::FFT_EQ_FREQ_SIZE - 2 * trim);
  setFixedHeight(h);
  setStyleSheet("border: 2px solid white");
}

void EqGraph::paintEvent(QPaintEvent* event) {
  const int barWidth = 1; // width() / Audio::FFT_EQ_FREQ_SIZE;
  const int maxH = h / 2 - 3;
  for (int i = trim; i < Audio::FFT_EQ_FREQ_SIZE - trim - 10; i++) {
    const int c = i % 2 == 0 ? 1 : -1;
    barHeightBuffer[i - trim] = c * std::min(static_cast<int>(eqBuffer[i] * (h / 2)), maxH);
  }
  QPainter painter(this);
  QPen pen(Qt::NoPen);
  if (!painter.isActive())
    painter.begin(this);
  painter.setPen(pen);
  for (int i = 0; i < Audio::FFT_EQ_FREQ_SIZE - 2 * trim; i++) {
    painter.fillRect(i, h / 2, barWidth, barHeightBuffer[i], Qt::white);
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