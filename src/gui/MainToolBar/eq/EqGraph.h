//
// Created by ns on 8/20/25.
//

#ifndef EQGRAPH_H
#define EQGRAPH_H

#include <algorithm>
#include <atomic>
#include <chrono>
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
#include "../../../audio/Mixer.h"
#include "../../../enums/PlayState.h"
#include "../../../enums/Result.h"

namespace Gj {
namespace Gui {

class EqGraph : public QOpenGLWidget, protected QOpenGLFunctions {

  public:
    EqGraph(QWidget* parent, Audio::Mixer* mixer);
    ~EqGraph();
    void setEqRingBuffer(jack_ringbuffer_t* eqRingBuffer);
    void animationStart();
    void animationStop();
    Result hydrateState(const AppStatePacket& appStatePacket);

  private slots:
    void animationLoop();

  private:
    int h = 75;
    int maxBarH = 30;
    float maxBarHf = 30.0f;
    Audio::Mixer* mixer;
    jack_ringbuffer_t* eqRingBuffer;
    std::mutex eqBufferMutex;
    float eqBuffer[Audio::FFT_EQ_RING_BUFFER_SIZE]{ 0.0f };
    unsigned int trim = 150;
    unsigned int barHeightBufferSize = 150;
    std::atomic<float> barHeightBuffer[Audio::FFT_EQ_FREQ_SIZE - 2 * 150]{ 0 };
    float vertices[(Audio::FFT_EQ_FREQ_SIZE - 2 * 150) * 6] { 0.0f };

    std::atomic<bool> stopEqWorkerThread = false;
    std::thread eqWorkerThread;
    QTimer animationTimer;
    GLuint vao, vbo;
    QOpenGLShaderProgram* program;

    void setStyle();
    // void paintEvent(QPaintEvent *event) override;
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
