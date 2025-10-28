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
  // TODO
}

void CurrentlyPlaying::hydrateState(const AppStatePacket& appStatePacket) {
  track.setText(QString(appStatePacket.currentlyPlayingTrackTitle.c_str()));
  album.setText(QString(appStatePacket.currentlyPlayingAlbumTitle.c_str()));
  artist.setText(QString(appStatePacket.currentlyPlayingArtistName.c_str()));
}

} // Gui
} // Gj