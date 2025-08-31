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
  , vao(0)
  , vbo(0)
  , program(nullptr)
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

EqGraph::~EqGraph() {
  animationStop();
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  delete program;
}

void EqGraph::initializeGL() {
  std::cout << "initializeGL" << std::endl;
  initializeOpenGLFunctions();
  glClearColor(0.0f, 0.20f, 0.20f, 1.0f);
  static const char* vertexShaderSource = R"(
      #version 330 core
      layout(location = 0) in vec2 position;
      void main() {
          gl_Position = vec4(position, 0.0, 1.0);
      }
  )";

  static const char* fragmentShaderSource = R"(
      #version 330 core
      out vec4 fragColor;
      void main() {
          fragColor = vec4(1.0, 0.0, 1.0, 1.0);
      }
  )";

  program = new QOpenGLShaderProgram();
  program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
  program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
  program->link();

  // Rectangle defined as two triangles
  float vertices[] = {
    -0.5f, -0.5f, // bottom-left
     0.5f, -0.5f, // bottom-right
     0.5f,  0.5f, // top-right

    -0.5f, -0.5f, // bottom-left
     0.5f,  0.5f, // top-right
    -0.5f,  0.5f  // top-left
  };

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

  glBindVertexArray(0);
}

void EqGraph::paintGL() {
  std::cout << "paintGL" << std::endl;

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  program->bind();
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
  program->release();
}

void EqGraph::resizeGL(int w, int h) {
  std::cout << "resizeGL. w: " << w << " h: " << h << std::endl;
  glViewport(0, 0, w, h);
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

//void EqGraph::paintEvent(QPaintEvent* event) {
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
//}

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