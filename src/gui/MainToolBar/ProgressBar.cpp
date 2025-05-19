//
// Created by ns on 5/18/25.
//

#include "ProgressBar.h"


namespace Gj {
namespace Gui {

ProgressBar::ProgressBar(QWidget* parent, sf_count_t frame)
  : QWidget(parent)
  , frame(frame)
  {

   setStyle();
}

void ProgressBar::setStyle() {
  setMinimumWidth(400);
  setMinimumHeight(20);
  setMaximumHeight(20);
  setStyleSheet("background-color: red; border-radius: 10px; border: 2px solid black");
}


} // Gui
} // Gj