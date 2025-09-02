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
  for (int i = 0; i < blockCount; i++) {
    if (i == blockCount - 2)
      program->setUniformValue(colorLocation, red);
    else if (i == blockCount - 2 || i == blockCount - 3)
      program->setUniformValue(colorLocation, yellow);
    else
      program->setUniformValue(colorLocation, green);

    // todo: if vol >= .1 * blockcount
    const float iF = static_cast<float>(i);
    const float blockHeight = 0.09f;
    const float gap = 0.01f;
    const float xLeft = -0.9f;
    const float xRight = 0.9f;
    const float yBottom = 1.0f - iF * (blockHeight + gap);
    const float yTop = 1.0f - iF * (2 * blockHeight);

    // top triangle
    vertices[0] = xLeft;
    vertices[1] = yTop;
    vertices[2] = xRight;
    vertices[3] = yTop;
    vertices[4] = xRight;
    vertices[5] = yBottom;

    // bottom triangle
    vertices[6] = xLeft;
    vertices[7] = yTop;
    vertices[8] = xRight;
    vertices[9] = yBottom;
    vertices[10] = xLeft;
    vertices[11] = yBottom;

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0,  2);
  }
  program->release();

}

void VuMeter::resizeGL(int w, int h) {
  glViewport(0, 0, w, h);
}

} // Gui
} // Gj