//
// Created by ns on 11/4/24.
//

#include "./MainWindow.h"

using namespace caf;

namespace Gj {
namespace Gui {

MainWindow::MainWindow(actor_system& actorSystem)
    : actorSystem(actorSystem)
    , container(this)
    , menuBar(new MenuBar(actorSystem, this))
    , transportControl(this, actorSystem)
    , grid(&container)
    , musicLibraryWindow(&container)
    , mixerWindow(&container)
    {

  setCentralWidget(&container);
  initGrid();
  setStyleSheet("background-color: blue;");
  addToolBar(Qt::TopToolBarArea, &transportControl);
  setUnifiedTitleAndToolBarOnMac(true);
  setWindowTitle("GrooveJr");
  resize(640, 480);
}

int MainWindow::hydrateState(const AppStatePacket& appStatePacket) {
    std::cout << "MainWindow : hydrateState: " << std::endl;
    transportControl.hydrateState(appStatePacket);
    return 0;
}

void MainWindow::initGrid() {
  grid.setVerticalSpacing(1);
  for (int i = 0; i < 5; i++) {
    grid.setRowMinimumHeight(i, 25);
    grid.setColumnMinimumWidth(i, 25);
  }
  grid.setColumnStretch(0, 1);

  grid.addWidget(&musicLibraryWindow, 2, 0, 1, -1);
  grid.addWidget(&mixerWindow, 5, 0, -1, -1);

  container.setLayout(&grid);
  container.setStyleSheet("background-color: red;");
}

} // Gui
} // Gj