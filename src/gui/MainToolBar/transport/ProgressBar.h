//
// Created by ns on 5/18/25.
//

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <sndfile.hh>
#include <QRect>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QWidget>

#include "../../../audio/Mixer.h"

namespace Gj {
namespace Gui {

class ProgressBar final : public QWidget {

  public:
    ProgressBar(QWidget* parent, Audio::Mixer* mixer, sf_count_t frame);
    void updateProgressBar(sf_count_t readCount, sf_count_t newFrame);

  private:
    sf_count_t totalFrames;
    sf_count_t frame;
    Audio::Mixer* mixer;
    QPainter painter;
    QPen pen;

    void setStyle();
    void paintEvent(QPaintEvent *event) override;

};

} // Gui
} // Gj



#endif //PROGRESSBAR_H
