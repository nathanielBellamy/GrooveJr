//
// Created by ns on 9/2/25.
//

#ifndef VUMETER_H
#define VUMETER_H

#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QtOpenGL/QOpenGLShaderProgram>

#include <QColor>
#include <QTimer>

#include <sndfile.hh>

#include "../../../../audio/Constants.h"
#include "../../../../audio/Mixer.h"
#include "../../../../enums/PlayState.h"
#include "../../../../enums/Result.h"

namespace Gj {
namespace Gui {
  constexpr int blockCount = 16;
  constexpr float blockHeight = 1.3f / static_cast<float>(blockCount);
  constexpr float gap = 0.7f / static_cast<float>(blockCount + 1);
  constexpr float xLeft = -0.75f;
  constexpr float xRight = 0.75f;
  constexpr QColor black = QColor(0, 0, 0, 255);
  constexpr QColor green = QColor(0, 255, 0, 255);
  constexpr QColor yellow = QColor(255, 255, 0, 255);
  constexpr QColor red = QColor(255, 0, 0, 255);

class VuMeter final : public QOpenGLWidget, private QOpenGLFunctions {

  public:
    VuMeter(QWidget* parent, Audio::Mixer* mixer);
    ~VuMeter();
    Result hydrateState(const AppStatePacket& appStatePacket);

  private slots:
    void animationLoop();

  private:
    int w = 25;
    float vertices[12] { 0.0f };
    GLuint vao, vbo;
    int colorLocation;
    QOpenGLShaderProgram* program;
    Audio::Mixer* mixer;
    QTimer animationTimer;

    void animationStart();
    void animationStop();
    void setStyle();
    void mousePressEvent(QMouseEvent* event) override;
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
};

} // Gui
} // Gj



#endif //VUMETER_H
