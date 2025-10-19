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

#include "../Color.h"
#include "../../enums/Result.h"
#include "../../Logging.h"
#include "../../AppState.h"
#include "../../enums/Result.h"

#include "MusicLibraryFilters.h"
#include "albums/AlbumTableView.h"
#include "artists/ArtistTableView.h"
#include "audioFiles/AudioFileTableView.h"
#include "genres/GenreTableView.h"
#include "playlists/PlaylistTableView.h"
#include "queue/QueueTableView.h"

using namespace caf;

namespace Gj {
namespace Gui {

enum MusicLibraryWindowMainSection {
  FILES,
  QUEUE
};

class MusicLibraryWindow final : public QWidget {
  actor_system& actorSystem;
  QGridLayout grid;
  MusicLibraryWindowMainSection mainSection = FILES;
  QPushButton filesButton;
  QPushButton queueButton;
  QLabel albumHeader;
  AlbumTableView* albumTableView;
  QLabel artistHeader;
  ArtistTableView* artistTableView;
  AudioFileTableView* audioFileTableView;
  QLabel genreHeader;
  GenreTableView* genreTableView;
  QLabel playlistHeader;
  PlaylistTableView* playlistTableView;
  QueueTableView* queueTableView;
  Result connectToDb();
  void setStyle();
  void setupGrid();
  Result connectActions();

  public:
    MusicLibraryFilters filters;
    explicit MusicLibraryWindow(QWidget *parent, actor_system& actorSystem);
    ~MusicLibraryWindow();

    Result hydrateState(const AppStatePacket& appStatePacket) {
      albumTableView->hydrateState(appStatePacket);
      artistTableView->hydrateState(appStatePacket);
      genreTableView->hydrateState(appStatePacket);
      playlistTableView->hydrateState(appStatePacket);

      if (audioFileTableView != nullptr)
        audioFileTableView->hydrateState(appStatePacket);
      if (queueTableView != nullptr)
        queueTableView->hydrateState(appStatePacket);
      return OK;
    };

    Result refresh() {
      albumTableView->refresh();
      artistTableView->refresh();
      genreTableView->refresh();
      playlistTableView->refresh();

      if (audioFileTableView != nullptr)
        audioFileTableView->refresh();
      if (queueTableView != nullptr)
        queueTableView->refresh();
      return OK;
    }
};

} // Gui
} // Gj



#endif //MUSICLIBRARYWINDOW_H
