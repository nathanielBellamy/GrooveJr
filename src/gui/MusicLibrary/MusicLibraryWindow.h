//
// Created by ns on 3/2/25.
//

#ifndef MUSICLIBRARYWINDOW_H
#define MUSICLIBRARYWINDOW_H

#include "caf/actor_system.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QString>

#include "../Color.h"
#include "../../enums/Result.h"
#include "../../Logging.h"
#include "../../AppState.h"
#include "../../enums/Result.h"
#include "../../db/Dao.h"
#include "../../db/entity/musicLibrary/Queue.h"

#include "MusicLibraryFilters.h"
#include "albums/AlbumTableView.h"
#include "artists/ArtistTableView.h"
#include "audioFiles/AudioFileTableView.h"
#include "cache/CacheTableView.h"
#include "genres/GenreTableView.h"
#include "playlists/PlaylistTableView.h"
#include "queue/QueueTableView.h"

using namespace caf;

namespace Gj {
namespace Gui {

enum MusicLibraryWindowMainSection {
  AUDIO_FILES_VIEW,
  CACHE_VIEW,
  QUEUE_VIEW
};

class MusicLibraryWindow final : public QWidget {
  actor_system& actorSystem;
  AppState* gAppState;
  Db::Dao* dao;
  QGridLayout grid;
  MusicLibraryWindowMainSection mainSection = AUDIO_FILES_VIEW;
  QPushButton cacheButton;
  QPushButton filesButton;
  QPushButton queueButton;
  QPushButton refreshButton;
  QLabel filtersHeader;
  QPushButton clearFiltersButton;
  QLabel albumHeader;
  AlbumTableView* albumTableView;
  QPushButton albumClearFilterButton;
  QLabel artistHeader;
  ArtistTableView* artistTableView;
  QPushButton artistClearFilterButton;
  AudioFileTableView* audioFileTableView;
  QLabel genreHeader;
  GenreTableView* genreTableView;
  QPushButton genreClearFilterButton;
  QLabel playlistHeader;
  PlaylistTableView* playlistTableView;
  QPushButton playlistClearFilterButton;
  QueueTableView* queueTableView;
  CacheTableView* cacheTableView;
  Result connectToDb();
  void setStyle();
  void setupGrid();
  Result connectActions();

  const QString STYLE_STR_BTN_SELECTED = QString(
    ("background-color: " + Color::toHex(GjC::LIGHT_400) + " ;").c_str()
  );

  const QString STYLE_STR_BTN_NOT_SELECTED = QString(
    ("background-color: " + Color::toHex(GjC::DARK_400) + " ;").c_str()
  );

  public:
    MusicLibraryFilters filters;
    explicit MusicLibraryWindow(QWidget *parent, actor_system& actorSystem, AppState* gAppState, Db::Dao* dao);
    ~MusicLibraryWindow();

    Result hydrateState(const AppStatePacket& appStatePacket) {
      albumTableView->hydrateState(appStatePacket);
      artistTableView->hydrateState(appStatePacket);
      cacheTableView->hydrateState(appStatePacket);
      genreTableView->hydrateState(appStatePacket);
      playlistTableView->hydrateState(appStatePacket);
      audioFileTableView->hydrateState(appStatePacket);
      queueTableView->hydrateState(appStatePacket);

      return OK;
    };

    Result refresh() {
      albumTableView->refresh();
      artistTableView->refresh();
      cacheTableView->refresh();
      genreTableView->refresh();
      playlistTableView->refresh();
      audioFileTableView->refresh();
      queueTableView->refresh();

      return OK;
    }

    Result showAsMainSection(MusicLibraryWindowMainSection newMainSection) {
      mainSection = newMainSection;
      switch (newMainSection) {
        case CACHE_VIEW:
          cacheButton.setStyleSheet(STYLE_STR_BTN_SELECTED);
          cacheTableView->show();
          filesButton.setStyleSheet(STYLE_STR_BTN_NOT_SELECTED);
          audioFileTableView->hide();
          queueButton.setStyleSheet(STYLE_STR_BTN_NOT_SELECTED);
          queueTableView->hide();
          break;
        case QUEUE_VIEW:
          cacheButton.setStyleSheet(STYLE_STR_BTN_NOT_SELECTED);
          cacheTableView->hide();
          filesButton.setStyleSheet(STYLE_STR_BTN_NOT_SELECTED);
          audioFileTableView->hide();
          queueButton.setStyleSheet(STYLE_STR_BTN_SELECTED);
          queueTableView->show();
          break;
        default: // case FILES:
          cacheButton.setStyleSheet(STYLE_STR_BTN_NOT_SELECTED);
          cacheTableView->hide();
          filesButton.setStyleSheet(STYLE_STR_BTN_SELECTED);
          audioFileTableView->show();
          queueButton.setStyleSheet(STYLE_STR_BTN_NOT_SELECTED);
          queueTableView->hide();
      }

      refresh();
      return OK;
    }
};

} // Gui
} // Gj



#endif //MUSICLIBRARYWINDOW_H
