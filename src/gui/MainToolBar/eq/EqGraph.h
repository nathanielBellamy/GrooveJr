//
// Created by ns on 8/20/25.
//

#ifndef EQGRAPH_H
#define EQGRAPH_H

#include <algorithm>
#include <atomic>
#include <chrono>
#include <vector>
#include <thread>

#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QMouseEvent>
#include <QRect>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QTimer>
#include <QWidget>

#include <sndfile.hh>
#include <jack/ringbuffer.h>

#include "../../../audio/Constants.h"
#include "../../../audio/mixer/Core.h"
#include "../../../enums/PlayState.h"
#include "../../../enums/Result.h"

namespace Gj {
namespace Gui {
constexpr int EG_GRAPH_TRIM = 125;
constexpr int EG_GRAPH_AVG_SIZE = 6;

class EqGraph final : public QOpenGLWidget, protected QOpenGLFunctions {
public:
  EqGraph(QWidget* parent, Audio::Mixer::Core* mixer);

  ~EqGraph();

  void setEqRingBuffer(jack_ringbuffer_t* eqRingBuffer);

  Result hydrateState(const AppStatePacket& appStatePacket);

private slots:
  void animationLoop();

private:
  bool running = false;
  int h = 126;
  int w = Audio::FFT_EQ_FREQ_SIZE - 2 * EG_GRAPH_TRIM;
  int maxBarH = 30;
  float maxBarHf = 30.0f;
  Audio::Mixer::Core* mixer;
  jack_ringbuffer_t* eqRingBuffer;
  float eqBuffer[Audio::FFT_EQ_RING_BUFFER_SIZE]{0.0f};
  std::atomic<int> avgIndex = 0;
  float barHeightBufferAvg[EG_GRAPH_AVG_SIZE][Audio::FFT_EQ_FREQ_SIZE - 2 * EG_GRAPH_TRIM]{0.0f};
  std::atomic<float> barHeightBuffer[Audio::FFT_EQ_FREQ_SIZE - 2 * EG_GRAPH_TRIM]{0.0f};
  float vertices[(Audio::FFT_EQ_FREQ_SIZE - 2 * EG_GRAPH_TRIM) * 6]{0.0f};

  int colorLocation;
  std::atomic<int> colorIndex = 0;
  std::atomic<int> backgroundColorIndex = 4;
  std::vector<QColor> colors = {
    QColor(0, 197, 170, 255),
    QColor(255, 0, 255, 255),
    QColor(24, 147, 224, 255),
    QColor(255, 255, 255, 255),
    QColor(0, 0, 0, 255),
  };

  std::atomic<bool> stopEqWorkerThread = false;
  std::thread eqWorkerThread;
  QTimer animationTimer;
  GLuint vao, vbo;
  QOpenGLShaderProgram* program;

  void animationStart();

  void animationStop();

  void setStyle();

  void mousePressEvent(QMouseEvent* event) override;

  void initializeGL() override;

  void paintGL() override;

  void resizeGL(int w, int h) override;

  void startWorker();

  void stopWorker();
};
} // Gui
} // Gj


#endif //EQGRAPH_H
