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

#include "../Color.h"
#include "../../enums/Result.h"
#include "../../Logging.h"
#include "../../AppState.h"
#include "../../enums/Result.h"

#include "albums/AlbumTableView.h"
#include "artists/ArtistTableView.h"
#include "audioFiles/AudioFileTableView.h"
#include "tracks/TrackTableView.h"

using namespace caf;

namespace Gj {
namespace Gui {

class MusicLibraryWindow final : public QWidget {
  public:
    explicit MusicLibraryWindow(QWidget *parent, actor_system& actorSystem);
    ~MusicLibraryWindow();
    Result hydrateState(const AppStatePacket& appStatePacket) {
      albumTableView->hydrateState(appStatePacket);
      artistTableView->hydrateState(appStatePacket);
      trackTableView->hydrateState(appStatePacket);
      return OK;
    };

  private:
    actor_system& actorSystem;
    QGridLayout grid;
    QLabel title;
    QLabel albumHeader;
    AlbumTableView* albumTableView;
    QLabel artistHeader;
    ArtistTableView* artistTableView;
    QLabel trackHeader;
    TrackTableView* trackTableView;
    QLabel audioFileHeader;
    AudioFileTableView* audioFileTableView;
    Result connectToDb();
    void setStyle();
    void setupGrid();
};

} // Gui
} // Gj



#endif //MUSICLIBRARYWINDOW_H
