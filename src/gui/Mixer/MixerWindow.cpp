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
  {

  auto cleanChannel = std::make_unique<Channel>(this, 1);
  channels.push_back(std::move(cleanChannel));

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
  grid.addWidget(channels.front().get(), 1, 0, -1, 1);
  grid.setRowStretch(0, 1);
  grid.setRowMinimumHeight(0, 20);
  grid.setRowStretch(1, 10);

  setLayout(&grid);
}

} // Gui
} // Gj