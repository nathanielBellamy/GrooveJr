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
  , albumHeader(this)
  , artistHeader(this)
  , genreHeader(this)
  , playlistHeader(this)
  , audioFileHeader(this)
  {
  if (connectToDb() == OK) {
    albumTableView = new AlbumTableView(this, &filters);
    artistTableView = new ArtistTableView(this, &filters);
    audioFileTableView = new AudioFileTableView(this, &filters);
    genreTableView = new GenreTableView(this, &filters);
    playlistTableView = new PlaylistTableView(this, &filters);
  } else {
    Logging::write(
      Warning,
      "Gui::MusicLibraryWindow::MusicLibraryWindow()",
      "Something went wrong while connecting to the db."
    );
  }

  albumHeader.setText("Albums");
  albumHeader.setFont({albumHeader.font().family(), 12});

  artistHeader.setText("Artists");
  artistHeader.setFont({artistHeader.font().family(), 12});

  audioFileHeader.setText("Files");
  audioFileHeader.setFont({audioFileHeader.font().family(), 12});

  genreHeader.setText("Genres");
  genreHeader.setFont({genreHeader.font().family(), 12});

  playlistHeader.setText("Playlists");
  playlistHeader.setFont({playlistHeader.font().family(), 12});

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

  grid.setRowStretch(0, 1);
  grid.setRowStretch(1, 4);
  grid.setRowStretch(2, 1);
  grid.setRowStretch(3, 4);

  grid.setColumnStretch(0, 1);
  grid.setColumnStretch(1, 1);
  grid.setColumnStretch(2, 4);

  grid.addWidget(&genreHeader, 0, 0, 1, 1);
  grid.addWidget(&playlistHeader, 0, 1, 1, 1);

  grid.addWidget(genreTableView, 1, 0, 1, 1);
  grid.addWidget(playlistTableView, 1, 1, 1, 1);

  grid.addWidget(&artistHeader, 2, 0, 1, 1);
  grid.addWidget(&albumHeader, 2, 1, 1, 1);

  grid.addWidget(artistTableView, 3, 0, 1, 1);
  grid.addWidget(albumTableView, 3, 1, 1, 1);

  grid.addWidget(&audioFileHeader, 0, 2, -1, -1);
  grid.addWidget(audioFileTableView, 0, 2, -1, -1);

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

  const auto audioFileClickedConnection = connect(audioFileTableView, &QTableView::clicked, this, [&] (const QModelIndex& index) {
      const QVariant audioFileId = audioFileTableView->model()->index(index.row(), 6).data();
      filters.set(AUDIO_FILE, audioFileId.toLongLong());

      refresh();
  });

  const auto genreClickedConnection = connect(genreTableView, &QTableView::clicked, this, [&] (const QModelIndex& index) {
      const QVariant genreId = genreTableView->model()->index(index.row(), 1).data();
      filters.set(GENRE, genreId.toLongLong());

      refresh();
  });

  const auto playlistClickedConnection = connect(playlistTableView, &QTableView::clicked, this, [&] (const QModelIndex& index) {
      const QVariant playlistId = playlistTableView->model()->index(index.row(), 1).data();
      filters.set(PLAYLIST, playlistId.toLongLong());

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
