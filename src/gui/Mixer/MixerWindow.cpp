//
// Created by ns on 2/28/25.
//

#include "MixerWindow.h"

namespace Gj {
namespace Gui {

MixerWindow::MixerWindow(QWidget* parent)
  : QWidget(parent)
  , grid(this)
  , title(this)
  , dryChannel(this)
  , effectsChannelContainer(this)
  {

  title.setText("Mixer");
  title.setFont({title.font().family(), 18});

  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setStyle();
  setupGrid();
}

void MixerWindow::setStyle() {
  setStyleSheet("background-color: green;");
}

void MixerWindow::setupGrid() {
  grid.setVerticalSpacing(1);

  grid.addWidget(&title, 0, 0, 1, -1);
  grid.addWidget(&dryChannel, 1, 0, -1, 1);
  grid.addWidget(&effectsChannelContainer, 1, 1, -1, -1);

  grid.setColumnStretch(0, 1);
  grid.setColumnStretch(1, 10);
  grid.setRowMinimumHeight(0, 20);
  grid.setRowStretch(1, 10);

  setLayout(&grid);
}

} // Gui
} // Gj