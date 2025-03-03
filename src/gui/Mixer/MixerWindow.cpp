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
  setStyle();

  auto cleanChannel = std::make_unique<Channel>(new Channel(this));
  channels.push_back(std::move(cleanChannel));

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  title.setAlignment(Qt::AlignLeft|Qt::AlignBottom);
  title.setText("Mixer");
  title.setFont({title.font().family(), 36});
}

void MixerWindow::setStyle() {
  setStyleSheet("background-color: green;");
}

void MixerWindow::setupGrid() {
  grid.setVerticalSpacing(1);
  for (int i = 0; i < 5; i++) {
    grid.setRowMinimumHeight(i, 25);
    grid.setColumnMinimumWidth(i, 25);
  }
  grid.addWidget(&title, 0, 0, 2, -1);
  grid.addWidget(channels.front().get(), 2, 0, 1, 1);
  grid.setColumnStretch(0, 1);
  setLayout(&grid);
}

} // Gui
} // Gj