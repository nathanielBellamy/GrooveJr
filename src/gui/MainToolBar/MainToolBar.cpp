//
// Created by ns on 5/18/25.
//

#include "MainToolBar.h"

using namespace caf;

namespace Gj {
namespace Gui {

MainToolBar::MainToolBar(QWidget* parent, actor_system& sys, Audio::Mixer* mixer)
  : QToolBar(parent)
  , sys(sys)
  , currentlyPlaying(this, sys, mixer)
  , transportControl(this, sys, mixer)
  {

  title.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  title.setText("GrooveJr");
  title.setFont({title.font().family(), 36});
  addWidget(&title);

  addSeparator();
  addSeparator();

  addWidget(&currentlyPlaying);

  addSeparator();
  addSeparator();

  addWidget(&transportControl);

}


int MainToolBar::hydrateState(const AppStatePacket& appStatePacket) {
  Logging::write(
    Info,
    "Gui::MainToolbar::hydrateState",
    "Received app state with playState - " + std::to_string(appStatePacket.playState)
  );

  currentlyPlaying.hydrateState(appStatePacket);
  transportControl.hydrateState(appStatePacket);

  return 0;
}

} // Gui
} // Gj
