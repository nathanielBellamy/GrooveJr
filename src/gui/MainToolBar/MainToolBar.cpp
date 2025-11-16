//
// Created by ns on 5/18/25.
//

#include "MainToolBar.h"

using namespace caf;

namespace Gj {
namespace Gui {

MainToolBar::MainToolBar(QWidget* parent, actor_system& sys, AppState* gAppState, Audio::Mixer* mixer, QAction* sceneLoadAction)
  : QToolBar(parent)
  , sys(sys)
  , scenes(this, sys, gAppState, mixer, sceneLoadAction)
  , currentlyPlaying(this, sys, mixer)
  , transportControl(this, sys, mixer)
  , eqGraph(this, mixer)
  {

  title.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  title.setText("GrooveJr");
  title.setFont({title.font().family(), 18});
  title.setStyleSheet("text-align: center;");

  addWidget(&title);
  addSeparator();
  addWidget(&currentlyPlaying);
  addSeparator();
  addWidget(&transportControl);
  addSeparator();
  addWidget(&eqGraph);
  addSeparator();
  addWidget(&scenes);
}

int MainToolBar::hydrateState(const AppStatePacket& appStatePacket) {
  Logging::write(
    Info,
    "Gui::MainToolbar::hydrateState",
    "Received app state with playState - " + std::to_string(appStatePacket.playState)
  );

  currentlyPlaying.hydrateState(appStatePacket);
  transportControl.hydrateState(appStatePacket);
  scenes.hydrateState(appStatePacket);
  eqGraph.hydrateState(appStatePacket);

  return 0;
}

} // Gui
} // Gj
