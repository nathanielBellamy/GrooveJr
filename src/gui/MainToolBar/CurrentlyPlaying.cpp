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
  , track(new QLabel)
  , trackScrollArea(new QScrollArea(this))
  , album(new QLabel)
  , albumScrollArea(new QScrollArea(this))
  , artist(new QLabel)
  , artistScrollArea(new QScrollArea(this))
  , grid(this)
  {

  track->setWordWrap(false);
  album->setWordWrap(false);
  artist->setWordWrap(false);

  trackScrollArea->setWidget(track);
  trackScrollArea->setWidgetResizable(true);
  trackScrollArea->setMaximumHeight(25);
  trackScrollArea->horizontalScrollBar()->setStyleSheet("QScrollBar {height:0px;}");
  albumScrollArea->setWidget(album);
  albumScrollArea->setWidgetResizable(true);
  albumScrollArea->setMaximumHeight(25);
  albumScrollArea->horizontalScrollBar()->setStyleSheet("QScrollBar {height:0px;}");
  artistScrollArea->setWidget(artist);
  artistScrollArea->setWidgetResizable(true);
  artistScrollArea->setMaximumHeight(25);
  artistScrollArea->horizontalScrollBar()->setStyleSheet("QScrollBar {height:0px;}");

  setupGrid();
  setStyle();
}

void CurrentlyPlaying::setupGrid() {
  grid.addWidget(trackScrollArea, 0, 0, 1, -1);
  grid.addWidget(artistScrollArea, 1, 0, 1, -1);
  grid.addWidget(albumScrollArea, 2, 0, 1, -1);

  setLayout(&grid);
}

void CurrentlyPlaying::setStyle() {
  setFixedWidth(226);
  setStyleSheet(
    "font-weight: 600; font-size: 14px; background-color: black;"
  );
}

void CurrentlyPlaying::hydrateState(const AppStatePacket& appStatePacket) const {
  track->setText(QString(appStatePacket.currentlyPlayingTrackTitle.c_str()));
  album->setText(QString(appStatePacket.currentlyPlayingAlbumTitle.c_str()));
  artist->setText(QString(appStatePacket.currentlyPlayingArtistName.c_str()));
}

} // Gui
} // Gj