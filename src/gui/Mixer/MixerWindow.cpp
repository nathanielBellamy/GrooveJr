//
// Created by ns on 2/28/25.
//

#include "MixerWindow.h"

namespace Gj {
namespace Gui {

MixerWindow::MixerWindow(QWidget* parent, actor_system& actorSystem, Audio::Mixer* mixer)
  : QWidget(parent)
  , actorSystem(actorSystem)
  , mixer(mixer)
  , grid(this)
  , title(this)
  , mainDryContainer(this, actorSystem, mixer)
  , effectsChannelsContainer(this, actorSystem, mixer)
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
  grid.addWidget(&mainDryContainer, 1, 0, -1, 1);
  grid.addWidget(&effectsChannelsContainer, 1, 1, -1, 1);

  grid.setColumnStretch(0, 1);
  grid.setColumnStretch(1, 10);
  grid.setRowMinimumHeight(0, 20);
  grid.setRowStretch(1, 10);

  setLayout(&grid);
}

} // Gui
} // Gj