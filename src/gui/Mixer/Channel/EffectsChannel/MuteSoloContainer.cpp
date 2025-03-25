//
// Created by ns on 3/8/25.
//

#include "MuteSoloContainer.h"

namespace Gj {
namespace Gui {

MuteSoloContainer::MuteSoloContainer(QWidget* parent)
  : QWidget(parent)
  , grid(this)
  , mute(this)
  , solo(this)
  , effects(this)
  {

  setupGrid();
}


void MuteSoloContainer::setupGrid() {
  grid.setVerticalSpacing(0);
  grid.setHorizontalSpacing(2);

  grid.addWidget(&mute, 0, 0, 1, 1);
  grid.addWidget(&solo, 0, 1, 1, 1);
  grid.addWidget(&effects, 0, 2, 1, 1);
}


} // Gui
} // Gj