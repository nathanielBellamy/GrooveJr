//
// Created by ns on 11/4/24.
//

#include "./MainWindow.h"

using namespace caf;

namespace Gj {
namespace Gui {

MainWindow::MainWindow(actor_system& actorSystem)
    : actorSystem(actorSystem)
    , menuBar(new MenuBar(actorSystem, this))
    , transportControl(this, actorSystem) {

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

} // Gui
} // Gj