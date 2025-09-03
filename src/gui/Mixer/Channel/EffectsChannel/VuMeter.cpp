//
// Created by ns on 9/2/25.
//

#include "VuMeter.h"

namespace Gj {
namespace Gui {

VuMeter::VuMeter(QWidget* parent, Audio::Mixer* mixer)
  : QOpenGLWidget(parent)
  , mixer(mixer)
  , vao(0)
  , vbo(0)
  , program(nullptr)
  {

  animationStart();
  setStyle();
}

VuMeter::~VuMeter() {
  delete program;
}

void VuMeter::setStyle() {
  setFixedWidth(w);
}

void VuMeter::mousePressEvent(QMouseEvent* event) {
  // todo
}

void VuMeter::initializeGL() {
  initializeOpenGLFunctions();
  glClearColor(0, 0, 0, 255);

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
  program->enableAttributeArray(vertexLocation);
  colorLocation = program->uniformLocation("color");

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
}

void VuMeter::paintGL() {
  glClearColor(0, 0, 0, 255);
  glClear(GL_COLOR_BUFFER_BIT);

  program->bind();

  for (int i = 0; i < VU_METER_BLOCK_COUNT; i++) {
    for (int chan = 0; chan < 2; chan++) {
      if (i == VU_METER_BLOCK_COUNT - 1)
        program->setUniformValue(colorLocation, VU_METER_RED);
      else if (i == VU_METER_BLOCK_COUNT - 2 || i == VU_METER_BLOCK_COUNT - 3)
        program->setUniformValue(colorLocation, VU_METER_YELLOW);
      else
        program->setUniformValue(colorLocation, VU_METER_GREEN);

      // todo: if vol >= .1 * blockcount
      const float iF = static_cast<float>(i);
      const float yBottom = -1.0f + iF * (VU_METER_BLOCK_HEIGHT + VU_METER_GAP) + VU_METER_GAP;
      const float yTop = yBottom + VU_METER_BLOCK_HEIGHT;

      const float left = chan ? VU_METER_X_RIGHT_LEFT : VU_METER_X_LEFT_LEFT;
      const float right = chan ? VU_METER_X_RIGHT_RIGHT : VU_METER_X_LEFT_RIGHT;
      // top triangle
      vertices[0] = left;
      vertices[1] = yTop;
      vertices[2] = right;
      vertices[3] = yTop;
      vertices[4] = right;
      vertices[5] = yBottom;

      // bottom triangle
      vertices[6] = left;
      vertices[7] = yTop;
      vertices[8] = right;
      vertices[9] = yBottom;
      vertices[10] = left;
      vertices[11] = yBottom;

      glBindVertexArray(vao);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
      glDrawArrays(GL_TRIANGLES, 0,  10);
    }
  }
  program->release();

}

void VuMeter::resizeGL(int w, int h) {
  glViewport(0, 0, w, h);
}

void VuMeter::animationLoop() {
  program->bind();
  program->release();
}

void VuMeter::animationStart() {
  animationTimer.setInterval(32);
  connect(&animationTimer, &QTimer::timeout, this, &VuMeter::animationLoop);
  animationTimer.start();
}

void VuMeter::animationStop() {
  animationTimer.stop();
}

Result VuMeter::hydrateState(const AppStatePacket& appStatePacket) {
  switch (appStatePacket.playState) {
    case PLAY:
//      animationStart();
      break;
    case PAUSE:
    case STOP:
//      animationStop();
      break;
  }

  return OK;
};

} // Gui
} // Gj