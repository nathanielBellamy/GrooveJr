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

int MainWindow::hydrateState(const Gj::AppStatePacket& appStatePacket) {
    std::cout << "MainWindow : hydrateState: " << std::endl;
    transportControl.hydrateState(appStatePacket);
    return 0;
}

void MainWindow::initGrid() {
  grid.setVerticalSpacing(1);
  grid.setRowMinimumHeight(0, 25);
  grid.setRowMinimumHeight(1, 25);
  grid.setRowMinimumHeight(2, 25);
  grid.setRowMinimumHeight(3, 25);
  grid.setRowMinimumHeight(4, 25);
  grid.setColumnMinimumWidth(0, 25);
  grid.setColumnMinimumWidth(1, 25);
  grid.setColumnMinimumWidth(2, 25);
  grid.setColumnMinimumWidth(3, 25);
  grid.setColumnMinimumWidth(4, 25);
  grid.setColumnStretch(0, 1);
  mixerWindow.setStyleSheet("background-color: green;");
  grid.addWidget(&mixerWindow, 5, 0, -1, -1);
  container.setLayout(&grid);
  container.setStyleSheet("background-color: red;");
}

} // Gui
} // Gj