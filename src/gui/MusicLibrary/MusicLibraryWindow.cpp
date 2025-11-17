//
// Created by ns on 3/2/25.
//

#include "MusicLibraryWindow.h"

namespace Gj {
namespace Gui {

MusicLibraryWindow::MusicLibraryWindow(
  QWidget* parent,
  actor_system& actorSystem,
  AppState* gAppState,
  Db::Dao* dao,
  SqlWorkerPool* sqlWorkerPool
  )
  : QWidget(parent)
  , actorSystem(actorSystem)
  , gAppState(gAppState)
  , dao(dao)
  , grid(this)
  , cacheButton(this)
  , filesButton(this)
  , queueButton(this)
  , filtersHeader(this)
  , clearFiltersButton(this)
  , albumHeader(this)
  , albumClearFilterButton(this)
  , artistHeader(this)
  , artistClearFilterButton(this)
  , genreHeader(this)
  , genreClearFilterButton(this)
  , playlistHeader(this)
  , playlistClearFilterButton(this)
  {

  createAndConnectTableViews(sqlWorkerPool);
  showAsMainSection(AUDIO_FILES_VIEW);

  cacheButton.setCursor(Qt::PointingHandCursor);
  filesButton.setCursor(Qt::PointingHandCursor);
  queueButton.setCursor(Qt::PointingHandCursor);
  albumClearFilterButton.setCursor(Qt::PointingHandCursor);
  artistClearFilterButton.setCursor(Qt::PointingHandCursor);
  genreClearFilterButton.setCursor(Qt::PointingHandCursor);
  playlistClearFilterButton.setCursor(Qt::PointingHandCursor);

  cacheButton.setText("Cache");
  filesButton.setText("Files");
  queueButton.setText("Queue");
  albumClearFilterButton.setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_TitleBarCloseButton));
  artistClearFilterButton.setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_TitleBarCloseButton));
  genreClearFilterButton.setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_TitleBarCloseButton));
  playlistClearFilterButton.setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_TitleBarCloseButton));
  clearFiltersButton.setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_TitleBarCloseButton));

  filtersHeader.setText("Filters");
  filtersHeader.setFont({filtersHeader.font().family(), 14});

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
  Logging::write(
    Info,
    "Gui::MusicLibraryWindow::MusicLibraryWindow()",
    "Instantiated MusicLibraryWindow"
  );
}

Result MusicLibraryWindow::createAndConnectTableViews(SqlWorkerPool* sqlWorkerPool) {
  albumTableView = new AlbumTableView(this, actorSystem, dao, gAppState, &filters, sqlWorkerPool);
  artistTableView = new ArtistTableView(this, actorSystem, dao, gAppState, &filters, sqlWorkerPool);
  audioFileTableView = new AudioFileTableView(this, actorSystem, gAppState, dao, &filters, sqlWorkerPool);
  cacheTableView = new CacheTableView(this, actorSystem, dao, gAppState, &filters, sqlWorkerPool);
  genreTableView = new GenreTableView(this, actorSystem, dao, gAppState, &filters, sqlWorkerPool);
  playlistTableView = new PlaylistTableView(this, actorSystem, dao, gAppState, &filters, sqlWorkerPool);
  queueTableView = new QueueTableView(this, actorSystem, dao, gAppState, &filters, sqlWorkerPool);

  sqlWorkerPool->connectClient(albumTableView->getModel());
  sqlWorkerPool->connectClient(artistTableView->getModel());
  sqlWorkerPool->connectClient(audioFileTableView->getModel());
  sqlWorkerPool->connectClient(cacheTableView->getModel());
  sqlWorkerPool->connectClient(genreTableView->getModel());
  sqlWorkerPool->connectClient(playlistTableView->getModel());
  sqlWorkerPool->connectClient(queueTableView->getModel());

  return OK;
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
  delete cacheTableView;
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
  grid.setRowStretch(1, 1);
  grid.setRowStretch(2, 4);
  grid.setRowStretch(3, 1);
  grid.setRowStretch(4, 4);

  grid.setColumnStretch(0, 1);
  grid.setColumnStretch(1, 1);
  grid.setColumnStretch(2, 1);
  grid.setColumnStretch(3, 1);
  grid.setColumnStretch(4, 1);
  grid.setColumnStretch(5, 1);
  grid.setColumnStretch(6, 1);
  grid.setColumnStretch(7, 1);
  grid.setColumnStretch(8, 2);
  grid.setColumnStretch(9, 2);
  grid.setColumnStretch(10, 2);
  grid.setColumnStretch(11, 2);
  grid.setColumnStretch(12, 2);
  grid.setColumnStretch(13, 2);
  grid.setColumnStretch(14, 2);
  grid.setColumnStretch(15, 2);

  grid.addWidget(&filtersHeader, 0, 0, 1, 3);
  grid.addWidget(&clearFiltersButton, 0, 7, 1, 1);

  grid.addWidget(&filesButton, 0, 8, 1, 1);
  grid.addWidget(&queueButton, 0, 9, 1, 1);
  grid.addWidget(&cacheButton, 0, 10, 1, 1);

  grid.addWidget(&genreHeader, 1, 0, 1, 1);
  grid.addWidget(&genreClearFilterButton, 1, 3, 1, 1);
  grid.addWidget(&playlistHeader, 1, 4, 1, 1);
  grid.addWidget(&playlistClearFilterButton, 1, 7, 1, 1);

  grid.addWidget(genreTableView, 2, 0, 1, 4);
  grid.addWidget(playlistTableView, 2, 4, 1, 4);

  grid.addWidget(&artistHeader, 3, 0, 1, 1);
  grid.addWidget(&artistClearFilterButton, 3, 3, 1, 1);
  grid.addWidget(&albumHeader, 3, 4, 1, 1);
  grid.addWidget(&albumClearFilterButton, 3, 7, 1, 1);

  grid.addWidget(artistTableView, 4, 0, 1, 4);
  grid.addWidget(albumTableView, 4, 4, 1, 4);

  // add overlapping mainSections to hide/show
  grid.addWidget(audioFileTableView, 1, 8, -1, -1);
  grid.addWidget(queueTableView, 1, 8, -1, -1);
  grid.addWidget(cacheTableView, 1, 8, -1, -1);

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

  const auto cacheButtonClickedConnection = connect(&cacheButton, &QPushButton::clicked, this, [&] () {
    showAsMainSection(CACHE_VIEW);
    refresh();
  });

  const auto clearFiltersButtonClickedConnection = connect(&clearFiltersButton, &QPushButton::clicked, this, [&] () {
    filters.filters.at(ALBUM).ids.clear();
    filters.filters.at(ARTIST).ids.clear();
    filters.filters.at(GENRE).ids.clear();
    filters.filters.at(PLAYLIST).ids.clear();
    refresh();
  });

  const auto clearAlbumFilterButtonClickedConnection = connect(&albumClearFilterButton, &QPushButton::clicked, this, [&] () {
    filters.filters.at(ALBUM).ids.clear();
    refresh();
  });

  const auto clearArtistFilterButtonClickedConnection = connect(&artistClearFilterButton, &QPushButton::clicked, this, [&] () {
    filters.filters.at(ARTIST).ids.clear();
    refresh();
  });

  const auto clearGenreFilterButtonClickedConnection = connect(&genreClearFilterButton, &QPushButton::clicked, this, [&] () {
    filters.filters.at(GENRE).ids.clear();
    refresh();
  });

  const auto clearPlaylistFilterButtonClickedConnection = connect(&playlistClearFilterButton, &QPushButton::clicked, this, [&] () {
    filters.filters.at(PLAYLIST).ids.clear();
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

  return OK;
}

} // Gui
} // Gj
