//
// Created by ns on 5/17/25.
//

#include "CurrentlyPlaying.h"

namespace Gj {
namespace Gui {

CurrentlyPlaying::CurrentlyPlaying(QWidget* parent, actor_system& actorSystem, Audio::Mixer* mixer)
  : QWidget(parent)
  , actorSystem(actorSystem)
  , mixer(mixer)
  , track(this)
  , album(this)
  , artist(this)
  , grid(this)
  {

  track.setText("Love is a Losing Game");
  album.setText("Back to Black");
  artist.setText("Amy Winehouse");

  grid.addWidget(&track, 0, 0, 1, -1);
  grid.addWidget(&album, 1, 0, 1, 1);
  grid.addWidget(&artist, 1, 1, 1, 1);

  setLayout(&grid);

  setStyle();
}

void CurrentlyPlaying::setStyle() {
  setStyleSheet("background-color: blue;");
}

void CurrentlyPlaying::hydrateState(const AppStatePacket& appStatePacket) {
  // TODO
}

} // Gui
} // Gj