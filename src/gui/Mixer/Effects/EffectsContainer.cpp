//
// Created by ns on 3/16/25.
//

#include "EffectsContainer.h"

namespace Gj {
namespace Gui {

EffectsContainer::EffectsContainer(QWidget* parent)
  : QWidget(parent)
  , grid(this)
  , title(this)
  , vstWindows()
  {

  title.setText("EffectsContainer");
  title.setFont({title.font().family(), 18});

  auto vstWindow1 = std::make_unique<VstWindow>(this);
  vstWindows.push_back(std::move(vstWindow1));

  setStyle();
  setupGrid();
}

void EffectsContainer::setStyle() {
  setStyleSheet("background-color: aqua;");
}

void EffectsContainer::setupGrid() {

  grid.setVerticalSpacing(1);

  grid.addWidget(&title, 0, 0, 1, -1);
  grid.addWidget(vstWindows.front().get(), 1, 0, 1, -1);

  grid.setColumnStretch(0, 1);
  grid.setColumnStretch(1, 10);
  grid.setRowMinimumHeight(0, 20);
  grid.setRowStretch(1, 10);


  setLayout(&grid);
}


} // Gui
} // Gj