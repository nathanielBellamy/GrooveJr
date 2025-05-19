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

#include <iostream>

namespace Gj {
namespace Gui {

class ProgressBar final : public QWidget {

  public:
    ProgressBar(QWidget* parent, sf_count_t frame);

  private:
    sf_count_t frame;
    QPainter painter;
    QPen pen;
    QRect elapsed;

    void setStyle();
    void paintEvent(QPaintEvent *event);

};

} // Gui
} // Gj



#endif //PROGRESSBAR_H
