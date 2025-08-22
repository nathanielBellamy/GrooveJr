//
// Created by ns on 8/20/25.
//

#ifndef EQGRAPH_H
#define EQGRAPH_H

#include <QMouseEvent>
#include <QRect>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QWidget>

#include <sndfile.hh>

#include "../../../audio/Constants.h"
#include "../../../audio/Mixer.h"
#include "../../../enums/PlayState.h"
#include "../../../enums/Result.h"

namespace Gj {
namespace Gui {

class EqGraph : public QWidget {

  public:
    EqGraph(QWidget* parent, Audio::Mixer* mixer);
    void updateEqGraph(float* newEqBuffer);
    Result hydrateState(const AppStatePacket& appStatePacket);

  private:
    Audio::Mixer* mixer;
    QPainter painter;
    QPen pen;
    float* eqBuffer;
    int trim = 150;

    void setStyle();
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent* event) override;
};

} // Gui
} // Gj



#endif //EQGRAPH_H
