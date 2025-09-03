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
#include <jack/ringbuffer.h>

#include "../../../../audio/Constants.h"
#include "../../../../audio/Mixer.h"
#include "../../../../enums/PlayState.h"
#include "../../../../enums/Result.h"

namespace Gj {
namespace Gui {
  constexpr int VU_METER_AVG_SIZE = 4;
  constexpr int VU_METER_BLOCK_COUNT = 16;
  constexpr float VU_METER_BLOCK_HEIGHT = 1.3f / static_cast<float>(VU_METER_BLOCK_COUNT);
  constexpr float VU_METER_GAP = 0.7f / static_cast<float>(VU_METER_BLOCK_COUNT + 1);
  constexpr float VU_METER_X_LEFT_LEFT = -0.9f;
  constexpr float VU_METER_X_LEFT_RIGHT = -0.1f;
  constexpr float VU_METER_X_RIGHT_LEFT = 0.1f;
  constexpr float VU_METER_X_RIGHT_RIGHT = 0.9f;
  constexpr auto VU_METER_BLACK = QColor(0, 0, 0, 255);
  constexpr auto VU_METER_GREEN = QColor(0, 255, 0, 255);
  constexpr auto VU_METER_YELLOW = QColor(255, 255, 0, 255);
  constexpr auto VU_METER_RED = QColor(255, 0, 0, 255);

class VuMeter final : public QOpenGLWidget, private QOpenGLFunctions {

  public:
    VuMeter(QWidget* parent, Audio::Mixer* mixer);
    ~VuMeter();
    Result hydrateState(const AppStatePacket& appStatePacket);

  private slots:
    void animationLoop();

  private:
    int w = 40;
    jack_ringbuffer_t* vuRingBuffer = nullptr;
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
