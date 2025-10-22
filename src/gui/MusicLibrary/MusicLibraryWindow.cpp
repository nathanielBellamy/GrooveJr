//
// Created by ns on 3/2/25.
//

#include "MusicLibraryWindow.h"

namespace Gj {
namespace Gui {

MusicLibraryWindow::MusicLibraryWindow(QWidget* parent, actor_system& actorSystem, Db::Dao* dao)
  : QWidget(parent)
  , actorSystem(actorSystem)
  , dao(dao)
  , grid(this)
  , filesButton(this)
  , queueButton(this)
  , albumHeader(this)
  , artistHeader(this)
  , genreHeader(this)
  , playlistHeader(this)
  {
  if (connectToDb() == OK) {
    albumTableView = new AlbumTableView(this, actorSystem, dao, &filters);
    artistTableView = new ArtistTableView(this, actorSystem, dao, &filters);
    audioFileTableView = new AudioFileTableView(this, actorSystem, dao, &filters);
    genreTableView = new GenreTableView(this, actorSystem, dao, &filters);
    playlistTableView = new PlaylistTableView(this, actorSystem, dao, &filters);
    queueTableView = new QueueTableView(this, actorSystem, dao, &filters);
    showAsMainSection(AUDIO_FILES_VIEW);
  } else {
    Logging::write(
      Warning,
      "Gui::MusicLibraryWindow::MusicLibraryWindow()",
      "Something went wrong while connecting to the db."
    );
  }

  filesButton.setCursor(Qt::PointingHandCursor);
  queueButton.setCursor(Qt::PointingHandCursor);
  refreshButton.setCursor(Qt::PointingHandCursor);

  queueButton.setText("Queue");
  filesButton.setText("Files");
  refreshButton.setText("Refresh");


  albumHeader.setText("Albums");
  albumHeader.setFont({albumHeader.font().family(), 12});

  artistHeader.setText("Artists");
  artistHeader.setFont({artistHeader.font().family(), 12});

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
  delete queueTableView;

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
  grid.setColumnStretch(2, 1);
  grid.setColumnStretch(3, 1);
  grid.setColumnStretch(4, 1);

  grid.addWidget(&filesButton, 0, 2, 1, 1);
  grid.addWidget(&queueButton, 0, 3, 1, 1);
  grid.addWidget(&refreshButton, 0, 4, 1, 1);

  grid.addWidget(&genreHeader, 0, 0, 1, 1);
  grid.addWidget(&playlistHeader, 0, 1, 1, 1);

  grid.addWidget(genreTableView, 1, 0, 1, 1);
  grid.addWidget(playlistTableView, 1, 1, 1, 1);

  grid.addWidget(&artistHeader, 2, 0, 1, 1);
  grid.addWidget(&albumHeader, 2, 1, 1, 1);

  grid.addWidget(artistTableView, 3, 0, 1, 1);
  grid.addWidget(albumTableView, 3, 1, 1, 1);

  // add overlapping mainSections to hide/show
  grid.addWidget(queueTableView, 1, 2, -1, -1);
  grid.addWidget(audioFileTableView, 1, 2, -1, -1);

  setLayout(&grid);
}

Result MusicLibraryWindow::connectActions() {
  const auto filesButtonClickedConnection = connect(&filesButton, &QPushButton::clicked, this, [&] () {
    showAsMainSection(AUDIO_FILES_VIEW);
    refresh();
  });

  const auto queueButtonClickedConnection = connect(&queueButton, &QPushButton::clicked, this, [&] () {
    showAsMainSection(QUEUE_VIEW);
    refresh();
  });

  const auto refreshButtonClickedConnection = connect(&refreshButton, &QPushButton::clicked, this, [&] () {
    refresh();
  });

  const auto albumClickedConnection = connect(albumTableView, &QTableView::clicked, this, [&] (const QModelIndex& index) {
      const QVariant albumId = albumTableView->getModel()->index(index.row(), 2).data();
      filters.set(ALBUM, albumId.toInt());

      refresh();
  });

  const auto artistClickedConnection = connect(artistTableView, &QTableView::clicked, this, [&] (const QModelIndex& index) {
      const QVariant artistId = artistTableView->getModel()->index(index.row(), 1).data();
      filters.set(ARTIST, artistId.toLongLong());

      refresh();
  });

  const auto audioFileClickedConnection = connect(audioFileTableView, &QTableView::clicked, this, [&] (const QModelIndex& index) {
      const QVariant audioFileId = audioFileTableView->getModel()->index(index.row(), 6).data();
      filters.set(AUDIO_FILE, audioFileId.toLongLong());

      refresh();
  });

  const auto audioFileDoubleClickedConnection = connect(audioFileTableView, &QTableView::doubleClicked, this, [&] (const QModelIndex& index) {
      const QVariant audioFileId = audioFileTableView->getModel()->index(index.row(), 6).data();

      // TODO: play

      refresh();
  });

  const auto genreClickedConnection = connect(genreTableView, &QTableView::clicked, this, [&] (const QModelIndex& index) {
      const QVariant genreId = genreTableView->getModel()->index(index.row(), 1).data();
      filters.set(GENRE, genreId.toLongLong());

      refresh();
  });

  const auto playlistClickedConnection = connect(playlistTableView, &QTableView::clicked, this, [&] (const QModelIndex& index) {
      const QVariant playlistId = playlistTableView->getModel()->index(index.row(), 1).data();
      filters.set(PLAYLIST, playlistId.toLongLong());

      refresh();
  });

  const auto playlistDoubleClickedConnection = connect(audioFileTableView, &QTableView::clicked, this, [&] (const QModelIndex& index) {
      const QVariant playlistId = playlistTableView->getModel()->index(index.row(), 1).data();

      // TODO: add to queue/play

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
