//
// Created by ns on 5/18/25.
//

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <cmath>

#include <sndfile.hh>

#include <QMouseEvent>
#include <QRect>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QWidget>

#include "../../../audio/Mixer.h"
#include "../../../enums/PlayState.h"
#include "../../../enums/Result.h"

namespace Gj {
namespace Gui {

class ProgressBar final : public QWidget {

  public:
    ProgressBar(QWidget* parent, Audio::Mixer* mixer, sf_count_t frameId);
    void updateProgressBar(sf_count_t frames, sf_count_t newFrameId);
    Result hydrateState(const AppStatePacket& appStatePacket);

  private:
    sf_count_t frames;
    sf_count_t frameId;
    Audio::Mixer* mixer;
    QPainter painter;
    QPen pen;

    void setStyle();
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent* event) override;


};

} // Gui
} // Gj



#endif //PROGRESSBAR_H
