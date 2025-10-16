//
// Created by ns on 10/13/25.
//

#include "PlaylistQueryModel.h"

namespace Gj {
namespace Gui {

Result PlaylistQueryModel::hydrateState(const AppStatePacket& appStatePacket) {
  // TODO
  // - if newQuery == query and refresh == false
  //   - no update
  // - else
  //   - setQuery(newQuery)
  //   - update()

  Logging::write(
      Info,
      "Gui::PlaylistQueryModel::hydrateState",
      "Setting Playlist Query: ===== newQuery ====="
  );
  return OK;
}

QVariant PlaylistQueryModel::data(const QModelIndex& index, int role) const {
  // TODO: format
  return QSqlQueryModel::data(index, role);
}

Result PlaylistQueryModel::refresh() {
  std::string queryStr;
  switch (filters->filterBy) {
    case ARTIST:
      queryStr = " select name from playlists p"
                 " join audioFile_to_playlists atp"
                 " on p.id = atp.playlistId"
                 " join audioFiles af"
                 " on af.id = atp.audioFileId"
                 " join track_to_artists tta"
                 " on af.trackId = tta.trackId"
                 " where tta.artistId in " + filters->idSqlArray();
      break;
    case GENRE:
      queryStr = " select name from playlists p"
                 " join audioFile_to_playlists atp"
                 " on p.id = atp.playlistId"
                 " join audioFiles af"
                 " on af.id = atp.audioFileId"
                 " join track_to_genres ttg"
                 " on af.trackId = ttg.trackId"
                 " where ttg.genreId in " + filters->idSqlArray();
      break;
    case TRACK:
      queryStr = " select name from playlists p"
                 " join audioFile_to_playlists atp"
                 " on p.id = atp.playlistId"
                 " join audioFiles af"
                 " on af.id = atp.audioFileId"
                 " where audioFiles.trackId in " + filters->idSqlArray();
      break;
    default:
      queryStr = "select name from playlists";
  }

  query = QString(queryStr.c_str());
  setQuery(query);
  setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));

  return OK;
}

} // Gui
} // Gj