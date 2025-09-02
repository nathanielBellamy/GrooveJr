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
  initializeOpenGLFunctions();
  glClearColor(0.0f, 0.20f, 0.20f, 1.0f);
  static const char* vertexShaderSource = R"(
      attribute highp vec4 vertex;
      // uniform highp mat4 matrix;
      void main(void) {
        gl_Position = vertex; // matrix * vertex;
      }
  )";

  static const char* fragmentShaderSource = R"(
    uniform mediump vec4 color;
    void main(void) {
       gl_FragColor = color;
    }
  )";

  program = new QOpenGLShaderProgram();
  program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
  program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
  program->link();
  program->bind();

  int vertexLocation = program->attributeLocation("vertex");
  int matrixLocation = program->uniformLocation("matrix");
  colorLocation = program->uniformLocation("color");

  program->enableAttributeArray(vertexLocation);
//  program->setAttributeArray(vertexLocation, vertices, 3);
//  program->setUniformValue(matrixLocation, pmvMatrix);
  program->setUniformValue(colorLocation, colors.at(colorIndex));

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

  glBindVertexArray(0);
}

void EqGraph::paintGL() {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  program->bind();
  program->setUniformValue(colorLocation, colors.at(colorIndex));
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
  glDrawArrays(GL_TRIANGLES, 0, (Audio::FFT_EQ_FREQ_SIZE - 2 * trim) * 6);
  program->release();
}

void EqGraph::resizeGL(int w, int h) {
  glViewport(0, 0, w, h);
}

void EqGraph::setEqRingBuffer(jack_ringbuffer_t* newEqRingBuffer) {
  eqRingBuffer = newEqRingBuffer;
}

void EqGraph::animationStart() {
  animationTimer.setInterval(32);
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


        for (int i = trim; i < Audio::FFT_EQ_FREQ_SIZE - trim - 1; i++) {
          const float c = i % 2 == 0 ? 1.0f : -1.0f;
          barHeightBuffer[i - trim].store(c * std::min(eqBuffer[i] * 10.0f, maxBarHf) / maxBarHf );
        }

        const float wF = static_cast<float>(Audio::FFT_EQ_FREQ_SIZE - 2 * trim);
        const float xZero = (Audio::FFT_EQ_FREQ_SIZE - 2 * trim) / 2;
        const float barWidth = 2.0f / wF;


        for (int i = 0; i < Audio::FFT_EQ_FREQ_SIZE - 2 * trim - 1; i += 2) {
          const int sixI = i * 6;
          const float x = static_cast<float>(i) * barWidth - 1.0f;
          // bottom triangle
          vertices[sixI] = x;
          vertices[sixI+1] = barHeightBuffer[i + 1];
          vertices[sixI+2] = x + barWidth;
          vertices[sixI+3] = barHeightBuffer[i + 1];
          vertices[sixI+4] = x + barWidth;
          vertices[sixI+5] = barHeightBuffer[i];

          // top triangle
          vertices[sixI+6] = x;
          vertices[sixI+7] = barHeightBuffer[i + 1];
          vertices[sixI+8] = x + barWidth;
          vertices[sixI+9] = barHeightBuffer[i];
          vertices[sixI+10] = x;
          vertices[sixI+11] = barHeightBuffer[i];
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

void EqGraph::mousePressEvent(QMouseEvent* event) {
  colorIndex = (colorIndex + 1) % colors.size();
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