//
// Created by ns on 10/13/25.
//

#include "PlaylistQueryModel.h"

namespace Gj {
namespace Gui {

Result PlaylistQueryModel::hydrateState(const AppStatePacket& appStatePacket) {
  Logging::write(
      Info,
      "Gui::PlaylistQueryModel::hydrateState",
      "PlaylistQueryModel::hydrateState"
  );
  return OK;
}

QVariant PlaylistQueryModel::data(const QModelIndex& index, int role) const {
  // TODO: format
  return QSqlQueryModel::data(index, role);
}

Result PlaylistQueryModel::refresh() {
  std::string queryStr;
  switch (filters->type) {
    case ALBUM:
      queryStr = " select name from playlists p"
                 " join audioFile_to_playlists atp"
                 " on p.id = atp.playlistId"
                 " join audioFiles af"
                 " on af.id = atp.audioFileId"
                 " join track_to_albums tta"
                 " on af.trackId = tta.trackId"
                 " where tta.albumId in " + filters->idSqlArray();
      break;
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
    case AUDIO_FILE:
      queryStr = " select name from playlists p"
                 " join audioFile_to_playlists atp"
                 " on p.id = atp.playlistId"
                 " join audioFiles af"
                 " on af.id = atp.audioFileId"
                 " where af.id in " + filters->idSqlArray();
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

  setQueryString(queryStr);
  setHeaderData(PLAYLIST_COL_NAME, Qt::Horizontal, QObject::tr("Name"));
  setHeaderData(PLAYLIST_COL_ID, Qt::Horizontal, QObject::tr("Id"));

  return OK;
}

} // Gui
} // Gj