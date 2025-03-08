//
// Created by ns on 3/2/25.
//

#include "DryChannel.h"

namespace Gj {
namespace Gui {

DryChannel::DryChannel(QWidget* parent)
  : QWidget(parent)
  , grid(this)
  , title(this)
  , slider(Qt::Vertical, this)
  {

  title.setText("Dry");
  title.setFont({title.font().family(), 16});

  slider.setMinimum(0);
  slider.setMaximum(127);
  slider.setTickInterval(1);
  slider.setTickPosition(QSlider::TicksRight);

  setupGrid();
  setStyle();
}

void DryChannel::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setStyleSheet("background-color: purple;");
}

void DryChannel::setupGrid() {
  grid.setVerticalSpacing(1);

  grid.addWidget(&title, 0, 0, 1, -1);
  grid.addWidget(&slider, 1, 0, -1, -1);

  grid.setRowStretch(0, 1);
  grid.setRowMinimumHeight(0, 20);
  grid.setRowMinimumHeight(1, 100);
  grid.setRowStretch(1, 10);

  setLayout(&grid);
}

} // Gui
} // Gj