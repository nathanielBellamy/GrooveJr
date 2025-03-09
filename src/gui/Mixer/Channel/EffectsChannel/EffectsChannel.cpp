//
// Created by ns on 3/7/25.
//

#include "EffectsChannel.h"

namespace Gj {
namespace Gui {

EffectsChannel::EffectsChannel(QWidget* parent, int channelIndex)
  : QWidget(parent)
  , channelIndex(channelIndex)
  , grid(this)
  , title(this)
  , slider(Qt::Vertical, this)
  , effectsSlots(this, channelIndex)
  , muteSoloContainer(this)
  {

  if (channelIndex == 0) {
    title.setText("Main");
  } else {
    title.setText("FX " + QString::number(channelIndex));
  }
  title.setFont({title.font().family(), 16});

  slider.setMinimum(0);
  slider.setMaximum(127);
  slider.setTickInterval(1);
  slider.setTickPosition(QSlider::NoTicks);

  setStyle();
  setupGrid();
}

void EffectsChannel::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setStyleSheet("background-color: orange;");
}

void EffectsChannel::setupGrid() {
  grid.setVerticalSpacing(1);

  grid.addWidget(&title, 0, 0, 1, -1);
  grid.addWidget(&slider, 1, 0, -1, -1);
  grid.addWidget(&effectsSlots, 1, 1, 1, 1);
  grid.addWidget(&muteSoloContainer, 2, 1, 1, 1);

  grid.setVerticalSpacing(2);
  grid.setHorizontalSpacing(2);

  grid.setColumnMinimumWidth(0, 20);
  grid.setRowStretch(0, 1);
  grid.setRowMinimumHeight(0, 20);
  grid.setRowMinimumHeight(1, 100);
  grid.setRowStretch(1, 10);

  setLayout(&grid);
}

} // Gui
} // Gj