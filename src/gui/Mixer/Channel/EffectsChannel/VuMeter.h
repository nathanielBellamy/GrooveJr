//
// Created by ns on 9/2/25.
//

#ifndef VUMETER_H
#define VUMETER_H

#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QtOpenGL/QOpenGLShaderProgram>

#include <QColor>

#include <sndfile.hh>

#include "../../../../audio/Constants.h"
#include "../../../../audio/Mixer.h"
#include "../../../../enums/PlayState.h"
#include "../../../../enums/Result.h"

namespace Gj {
namespace Gui {
  constexpr int blockCount = 10;
  constexpr QColor black = QColor(0, 0, 0, 255);
  constexpr QColor green = QColor(0, 255, 0, 255);
  constexpr QColor yellow = QColor(255, 255, 0, 255);
  constexpr QColor red = QColor(255, 0, 0, 255);

class VuMeter : public QOpenGLWidget, protected QOpenGLFunctions {

  public:
    VuMeter(QWidget* parent, Audio::Mixer* mixer);
    ~VuMeter();

  private:
    int w = 15;
    float vertices[12] { 0.0f };
    GLuint vao, vbo;
    int colorLocation;
    QOpenGLShaderProgram* program;
    Audio::Mixer* mixer;

    void setStyle();
    void mousePressEvent(QMouseEvent* event) override;
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
};

} // Gui
} // Gj



#endif //VUMETER_H
