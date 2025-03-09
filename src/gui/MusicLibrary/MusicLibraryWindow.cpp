//
// Created by ns on 3/2/25.
//

#include "MusicLibraryWindow.h"

namespace Gj {
namespace Gui {

MusicLibraryWindow::MusicLibraryWindow(QWidget* parent, actor_system& actorSystem)
  : QWidget(parent)
  , actorSystem(actorSystem)
  , grid(this)
  , title(this)
  {
  title.setText("Music Library");
  title.setFont({title.font().family(), 18});

  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  setStyle();
  setupGrid();
}

void MusicLibraryWindow::setStyle() {
  setStyleSheet("background-color: cyan;");
}

void MusicLibraryWindow::setupGrid() {
  grid.setVerticalSpacing(1);

  grid.addWidget(&title, 0, 0, 1, -1);
  grid.setRowStretch(0, 1);
  grid.setRowMinimumHeight(0, 20);

  setLayout(&grid);
}

} // Gui
} // Gj
