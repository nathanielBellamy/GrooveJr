//
// Created by ns on 3/2/25.
//

#include "MusicLibraryWindow.h"

namespace Gj {
namespace Gui {

MusicLibraryWindow::MusicLibraryWindow(QWidget* parent, actor_system& actorSystem)
  : QWidget(parent)
  , actorSystem(actorSystem)
  , grid(this)
  , title(this)
  , albumHeader(this)
  , artistHeader(this)
  , genreHeader(this)
  , playlistHeader(this)
  , trackHeader(this)
  , audioFileHeader(this)
  {
  if (connectToDb() == OK) {
    albumTableView = new AlbumTableView(this, &filters);
    artistTableView = new ArtistTableView(this, &filters);
    audioFileTableView = new AudioFileTableView(this, &filters);
    genreTableView = new GenreTableView(this);
    playlistTableView = new PlaylistTableView(this);
    trackTableView = new TrackTableView(this, &filters);
  } else {
    Logging::write(
      Warning,
      "Gui::MusicLibraryWindow::MusicLibraryWindow()",
      "Something went wrong while connecting to the db."
    );
  }

  title.setText("Music Library");
  title.setFont({title.font().family(), 18});

  albumHeader.setText("Albums");
  albumHeader.setFont({title.font().family(), 14});

  artistHeader.setText("Artists");
  artistHeader.setFont({title.font().family(), 14});

  audioFileHeader.setText("Audio Files");
  audioFileHeader.setFont({title.font().family(), 14});

  genreHeader.setText("Genre");
  genreHeader.setFont({title.font().family(), 14});

  playlistHeader.setText("Playlist");
  playlistHeader.setFont({title.font().family(), 14});

  trackHeader.setText("Tracks");
  trackHeader.setFont({title.font().family(), 14});

  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  connectActions();
  setStyle();
  setupGrid();
}

MusicLibraryWindow::~MusicLibraryWindow() {
  Logging::write(
    Info,
    "Gui::MusicLibraryWindow::~MusicLibraryWindow()",
    "Deleting MusicLibraryWindow"
  );

  delete albumTableView;
  delete artistTableView;
  delete audioFileTableView;
  delete genreTableView;
  delete playlistTableView;
  delete trackTableView;

  Logging::write(
    Info,
    "Gui::MusicLibraryWindow::~MusicLibraryWindow()",
    "Deleted MusicLibraryWindow"
  );
}

void MusicLibraryWindow::setStyle() {
  setStyleSheet((
    "background-color: " + Color::toHex(GjC::DARK_400) + "; ").data()
  );
}

void MusicLibraryWindow::setupGrid() {
  grid.setVerticalSpacing(1);

  grid.addWidget(&title, 0, 0, 1, 1);

  grid.setColumnStretch(0, 1);
  grid.setColumnStretch(1, 2);
  grid.setColumnStretch(2, 2);
  grid.setColumnStretch(3,3);
  grid.setColumnStretch(4, 4);
  grid.setColumnStretch(5, 2);

  grid.addWidget(&genreHeader, 1, 0, 1, 1);
  grid.addWidget(&playlistHeader, 1, 1, 1, 1);
  grid.addWidget(&artistHeader, 1, 2, 1, 1);
  grid.addWidget(&albumHeader, 1, 3, 1, 1);
  grid.addWidget(&trackHeader, 1, 4, 1, 1);
  grid.addWidget(&audioFileHeader, 1, 5, 1, 1);

  grid.addWidget(genreTableView, 2, 0, 5, 1);
  grid.addWidget(playlistTableView, 2, 1, 5, 1);
  grid.addWidget(artistTableView, 2, 2, 5, 1);
  grid.addWidget(albumTableView, 2, 3, 5, 1);
  grid.addWidget(trackTableView, 2, 4, 5, 1);
  grid.addWidget(audioFileTableView, 2, 5, 5, 1);

  setLayout(&grid);
}

Result MusicLibraryWindow::connectActions() {
  const auto albumClickedConnection = connect(albumTableView, &QTableView::clicked, this, [&] (const QModelIndex& index) {
      const QVariant albumId = albumTableView->model()->index(index.row(), 2).data();
      filters.set(ALBUM, albumId.toInt());

      refresh();
  });

  const auto artistClickedConnection = connect(artistTableView, &QTableView::clicked, this, [&] (const QModelIndex& index) {
      const QVariant artistId = artistTableView->model()->index(index.row(), 1).data();
      filters.set(ARTIST, artistId.toLongLong());

      refresh();
  });

  const auto trackClickedConnection = connect(trackTableView, &QTableView::clicked, this, [&] (const QModelIndex& index) {
      const QVariant trackId = trackTableView->model()->index(index.row(), 2).data();
      filters.set(TRACK, trackId.toLongLong());

      refresh();
  });

  return OK;
}

Result MusicLibraryWindow::connectToDb() {
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName("/Users/ns/groovejr.db");

  if (!db.open()) {
    Logging::write(
      Error,
      "Gui::MusicLibraryWindow::connectToDb",
      "Failed to connect to the database"
    );
    return ERROR;
  }

  Logging::write(
    Info,
      "Gui::MusicLibraryWindow::connectToDb",
    "MusicLibraryWindow Successfully Connected to Db"
  );

  return OK;
}

} // Gui
} // Gj
