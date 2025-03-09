//
// Created by ns on 11/4/24.
//

#include "./MainWindow.h"

using namespace caf;

namespace Gj {
namespace Gui {

MainWindow::MainWindow(actor_system& actorSystem, void (*shutdown_handler) (int))
    : actorSystem(actorSystem)
    , shutdown_handler(shutdown_handler)
    , container(this)
    , menuBar(new MenuBar(actorSystem, this))
    , transportControl(this, actorSystem)
    , grid(&container)
    , musicLibraryWindow(&container)
    , mixerWindow(&container, actorSystem)
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
  grid.setColumnStretch(0, 1);

  grid.addWidget(&musicLibraryWindow, 0, 0, 1, -1);
  grid.addWidget(&mixerWindow, 1, 0, -1, -1);
  for (int i = 0; i < 2; i++) {
    grid.setRowMinimumHeight(i, 200);
    grid.setRowStretch(i, 1);
  }

  container.setLayout(&grid);
  container.setStyleSheet("background-color: red;");
}

void MainWindow::closeEvent (QCloseEvent* e) {
  std::cout << "MainWindow::closeEvent " << std::endl;
  shutdown_handler(3);
}

} // Gui
} // Gj