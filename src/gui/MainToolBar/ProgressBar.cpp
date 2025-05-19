//
// Created by ns on 5/18/25.
//

#include "ProgressBar.h"


namespace Gj {
namespace Gui {

ProgressBar::ProgressBar(QWidget* parent, sf_count_t frame)
  : QWidget(parent)
  , frame(frame)
  , painter(this)
  , pen(Qt::NoPen)
  {

  setStyle();
}

void ProgressBar::setStyle() {
  setMinimumWidth(100);
  setFixedHeight(15);
  setStyleSheet("border: 2px solid white");
}

void ProgressBar::paintEvent(QPaintEvent* event) {
  std::cout << "progress bar paint event" << std::endl;
  painter.begin(this);
  painter.setPen(pen);
  painter.fillRect(0, 0, width() / 2, height(), Qt::white);
  painter.end();
}


} // Gui
} // Gj