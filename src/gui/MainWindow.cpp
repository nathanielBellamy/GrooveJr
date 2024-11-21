//
// Created by ns on 11/4/24.
//

#include "./MainWindow.h"

using namespace caf;

namespace Gs {
namespace Gui {

MainWindow::MainWindow(actor_system& sys)
    : sys(sys)
    , transportControl(this, sys)
    {

  label.setText("Welcome to\nGrooveSprings.");
  label.setFont({label.font().family(), 36});


  addToolBar(Qt::BottomToolBarArea, &transportControl);
  setCentralWidget(&frame);
  setUnifiedTitleAndToolBarOnMac(true);
  setWindowTitle("GrooveSprings");
  resize(640, 480);
}

int MainWindow::hydrateState(const Gs::AppStatePacket& appStatePacket) {
    std::cout << "MainWindow : hydrateState: " << std::endl;
    transportControl.hydrateState(appStatePacket);
    return 0;
}

} // Gui
} // Gs