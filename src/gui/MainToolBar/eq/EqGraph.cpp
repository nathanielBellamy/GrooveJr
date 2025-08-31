//
// Created by ns on 8/20/25.
//

#include "EqGraph.h"

namespace Gj {
namespace Gui {

EqGraph::EqGraph(QWidget* parent, Audio::Mixer* mixer)
  : QOpenGLWidget(parent)
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

void EqGraph::initializeGL() {
  std::cout << "initializeGL" << std::endl;
  initializeOpenGLFunctions();
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void EqGraph::paintGL() {
  std::cout << "paintGL" << std::endl;

//  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//  glColor3f(1.0f, 0.0f, 0.0f);
//
//  glBegin(GL_QUADS);

}

void EqGraph::resizeGL(int w, int h) {
  std::cout << "resizeGL. w: " << w << " h: " << h << std::endl;
  glViewport(0, 0, w, h);
}

EqGraph::~EqGraph() {
  animationStop();
}

void EqGraph::setEqRingBuffer(jack_ringbuffer_t* newEqRingBuffer) {
  eqRingBuffer = newEqRingBuffer;
}

void EqGraph::animationStart() {
  animationTimer.setInterval(64);
  connect(&animationTimer, &QTimer::timeout, this, &EqGraph::animationLoop);
  animationTimer.start();
}

void EqGraph::animationStop() {
  stopWorker();
  animationTimer.stop();
}

void EqGraph::animationLoop() {
  if (eqRingBuffer == nullptr)
    return;

  update();
}

void EqGraph::startWorker() {
  stopEqWorkerThread.store(false);
  eqWorkerThread = std::thread([this]() {
    while (!stopEqWorkerThread.load()) {
      if (eqRingBuffer == nullptr)
        continue;

      if (jack_ringbuffer_read_space(eqRingBuffer) > Audio::FFT_EQ_RING_BUFFER_SIZE - 2) {
        jack_ringbuffer_read(
          eqRingBuffer,
          reinterpret_cast<char*>(eqBuffer),
          Audio::FFT_EQ_RING_BUFFER_SIZE
        );

        for (int i = trim; i < Audio::FFT_EQ_FREQ_SIZE - trim - 10; i++) {
          const int c = i % 2 == 0 ? 1 : -1;
          barHeightBuffer[i - trim].store(c * std::min(static_cast<int>(eqBuffer[i] * 10), maxBarH));
        }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  });
  eqWorkerThread.detach();
}

void EqGraph::stopWorker() {
  stopEqWorkerThread.store(true);
}

void EqGraph::setStyle() {
  setFixedWidth(Audio::FFT_EQ_FREQ_SIZE - 2 * trim);
  setFixedHeight(h);
  setStyleSheet("border: 2px solid white");
}

void EqGraph::paintEvent(QPaintEvent* event) {
//  QPainter painter(this);
//  QPen pen(Qt::NoPen);
//  if (!painter.isActive())
//    painter.begin(this);
//  painter.setPen(pen);
//  for (int i = 0; i < Audio::FFT_EQ_FREQ_SIZE - 2 * trim; i += 16) {
////    painter.drawLine(QLine(trim, 0, trim, 0, h));
//    painter.fillRect(i, h / 2, 8, barHeightBuffer[i].load(), Qt::white);
//    painter.fillRect(i, h / 2, 8, barHeightBuffer[i+1].load(), Qt::white);
//  }
//  painter.end();
}

void EqGraph::mousePressEvent(QMouseEvent* event) {
  // todo
}

Result EqGraph::hydrateState(const AppStatePacket& appStatePacket) {
  switch (appStatePacket.playState) {
    case PLAY:
      startWorker();
      break;
    case PAUSE:
    case STOP:
      stopWorker();
      break;
  }

  return OK;
}

} // Gui
} // Gj