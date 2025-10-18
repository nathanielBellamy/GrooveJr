//
// Created by ns on 10/17/25.
//

#ifndef MUSICLIBRARYTABLEVIEW_H
#define MUSICLIBRARYTABLEVIEW_H

#include <QTableView>

#include "../../enums/Result.h"
#include "../../AppState.h"

#include "MusicLibraryQueryModel.h"
#include "MusicLibraryFilters.h"

#include "albums/AlbumQueryModel.h"
#include "audioFiles/AudioFileQueryModel.h"
#include "artists/ArtistQueryModel.h"
#include "genres/GenreQueryModel.h"
#include "playlists/PlaylistQueryModel.h"

namespace Gj {
namespace Gui {

class MusicLibraryTableView : public QTableView {
  Result setStyle() {
    setStyleSheet(
      "font-weight: 500; font-size: 12px;"
    );
    return OK;
  }

  protected:
    MusicLibraryQueryModel* model;

  public:
    MusicLibraryTableView(QWidget* parent, MusicLibraryFilters* filters, const MusicLibraryType type)
        : QTableView(parent)
        {

      switch (type) {
        case ALBUM:
          model = new AlbumQueryModel(parent, filters);
          break;
        case ARTIST:
          model = new ArtistQueryModel(parent, filters);
          break;
        case AUDIO_FILE:
          model = new AudioFileQueryModel(parent, filters);
          break;
        case GENRE:
          model = new GenreQueryModel(parent, filters);
          break;
        case PLAYLIST:
          model = new PlaylistQueryModel(parent, filters);
          break;
        default:
          model = nullptr;
      }

      setModel(model);
      setStyle();
    };

    ~MusicLibraryTableView() {
      delete model;
    }

    MusicLibraryQueryModel* getModel() const {
      return model;
    }

    Result hydrateState(const AppStatePacket& appStatePacket) const {
      model->hydrateState(appStatePacket);
      return OK;
    }

    Result refresh() const {
      model->refresh();
      return OK;
    }
};

} // Gui
} // Gj

#endif //MUSICLIBRARYTABLEVIEW_H
