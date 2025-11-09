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

QVariant GenreQueryModel::data(const QModelIndex& item, const int role) const {
  // TODO: format
  return QStandardItemModel::data(item, role);
}

Result GenreQueryModel::refresh() {
  std::string queryStr =
    " select g.name, g.id from genres g"
    " left outer join track_to_genres ttg"
    " on g.id = ttg.genreId"
    " left outer join tracks trk"
    " on trk.id = ttg.trackId"
    " left outer join track_to_artists tta"
    " on trk.id = tta.trackId"
    " left outer join audioFiles af"
    " on af.trackId = trk.id"
    " left outer join audioFile_to_playlists atp"
    " on af.id = atp.audioFileId"
    " where true";

  if (filters->filters.at(ALBUM).ids.size() > 0)
    queryStr += " and trk.albumId in " + filters->idSqlArray(ALBUM);

  if (filters->filters.at(ARTIST).ids.size() > 0)
    queryStr += " and tta.artistId in " + filters->idSqlArray(ARTIST);

  if (filters->filters.at(PLAYLIST).ids.size() > 0)
    queryStr += " and atp.playlistId in " + filters->idSqlArray(PLAYLIST);

  queryStr +=
    " group by g.id"
    " order by g.name";

  emit runQuery(id, QString(queryStr.c_str()));
  setHeaderData(GENRE_COL_ID, Qt::Horizontal, QObject::tr("Id"));
  setHeaderData(GENRE_COL_NAME, Qt::Horizontal, QObject::tr("Name"));

  return OK;
}

} // Gui
} // Gj