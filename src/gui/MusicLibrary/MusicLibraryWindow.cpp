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
  {
  if (connectToDb() == OK) {
    albumTableView = new AlbumTableView(this);
    artistTableView = new ArtistTableView(this);
    trackTableView = new TrackTableView(this);
  } else {
    Logging::write(
      Warning,
      "Gui::MusicLibraryWindow::MusicLibraryWindow()",
      "Something went wrong while connecting to the db."
    );
  }

  title.setText("Music Library");
  title.setFont({title.font().family(), 18});

  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
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
  grid.addWidget(artistTableView, 1, 0, 5, 2);
  grid.addWidget(albumTableView, 1, 2, 5, 2);
  grid.addWidget(trackTableView, 1, 4, 5, 2);
  grid.setRowStretch(0, 1);
  grid.setRowMinimumHeight(0, 20);
  // grid.setColumnStretch(0, 1);
  grid.setColumnMinimumWidth(0, 20);

  setLayout(&grid);
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
