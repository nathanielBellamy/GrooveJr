//
// Created by ns on 5/18/25.
//

#include "MainToolBar.h"

using namespace caf;

namespace Gj {
namespace Gui {
MainToolBar::MainToolBar(QWidget* parent, actor_system& sys, State::Core* stateCore, Audio::Mixer::Core* mixer,
                         SqlWorkerPool* sqlWorkerPool, QAction* sceneLoadAction)
: QToolBar(parent)
  , sys(sys)
  , scenes(new Scenes(this, sys, stateCore, mixer, sqlWorkerPool, sceneLoadAction))
  , currentlyPlaying(new CurrentlyPlaying(this, sys, mixer))
  , transportControl(new TransportControl(this, sys, mixer))
  , eqGraph(new EqGraph(this, mixer)) {
  title->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  title->setText("GrooveJr");
  title->setFont({title->font().family(), 18});
  title->setStyleSheet("text-align: center;");

  addWidget(title);
  addSeparator();
  addWidget(currentlyPlaying);
  addSeparator();
  addWidget(transportControl);
  addSeparator();
  addWidget(eqGraph);
  addSeparator();
  addWidget(scenes);

  // TODO: make it here
  Logging::write(
    Info,
    "Gui::MainToolBar::MainToolBar",
    "Instantiated MainToolBar."
  );
}

MainToolBar::~MainToolBar() {
  delete title;
  delete currentlyPlaying;
  delete transportControl;
  delete eqGraph;
}

Result MainToolBar::hydrateState(const State::Packet& statePacket) const {
  Logging::write(
    Info,
    "Gui::MainToolbar::hydrateState",
    "Received app state with playState - " + std::to_string(statePacket.playState)
  );

  currentlyPlaying->hydrateState(statePacket);
  transportControl->hydrateState(statePacket);
  scenes->hydrateState(statePacket);
  eqGraph->hydrateState(statePacket);

  return OK;
}
} // Gui
} // Gj
