//
// Created by ns on 10/11/25.
//

#include "GenreQueryModel.h"

namespace Gj {
namespace Gui {

Result GenreQueryModel::hydrateState(const AppStatePacket& appStatePacket) {
  Logging::write(
      Info,
      "Gui::GenreQueryModel::hydrateState",
      "GenreQueryModel::hydrateState"
  );
  return OK;
}

QVariant GenreQueryModel::data(const QModelIndex& index, int role) const {
  // TODO: format
  return QSqlQueryModel::data(index, role);
}

Result GenreQueryModel::refresh() {
  std::string queryStr;
  switch (filters->filterBy) {
    case ALBUM:
      queryStr = " select g.name, g.id from genres g"
                 " join track_to_genres ttg"
                 " on g.id = ttg.genreId"
                 " join tracks trk"
                 " on ttg.trackId = trk.id"
                 " where trk.albumId in " + filters->idSqlArray() +
                 " group by g.name";
      break;
    case ARTIST:
      queryStr = " select g.name, g.id from genres g"
                 " join track_to_genres ttg"
                 " on g.id = ttg.genreId"
                 " join track_to_artists tta"
                 " on ttg.trackId = tta.trackId"
                 " where tta.artistId in " + filters->idSqlArray() +
                 " group by g.name";
      break;
    case PLAYLIST:
      queryStr = " select g.name, g.id from genres g"
                 " join track_to_genres ttg"
                 " on g.id = ttg.genreId"
                 " join audioFiles af"
                 " on ttg.trackId = af.trackId"
                 " join audioFile_to_playlists atp"
                 " on af.id = atp.audioFileId"
                 " where atp.playlistId in " + filters->idSqlArray();
      break;
    case TRACK:
      queryStr = " select g.name, g.id from genres g"
                 " join track_to_genres ttg"
                 " on g.id = ttg.genreId"
                 " where ttg.trackId in " + filters->idSqlArray() +
                 " group by g.id";
      break;
    default:
      queryStr = "select name, id from genres";
  }

  query = QString(queryStr.c_str());
  setQuery(query);
  setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
  setHeaderData(0, Qt::Horizontal, QObject::tr("Id"));

  return OK;
}

} // Gui
} // Gj