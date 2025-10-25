//
// Created by ns on 10/11/25.
//

#include "QueueQueryModel.h"

namespace Gj {
namespace Gui {

Result QueueQueryModel::hydrateState(const AppStatePacket& appStatePacket) {
  Logging::write(
      Info,
      "Gui::QueueQueryModel::hydrateState",
      "QueueQueryModel::hydrateState"
  );
  return OK;
}

QVariant QueueQueryModel::data(const QModelIndex& index, int role) const {
  // TODO: format
  return QSqlQueryModel::data(index, role);
}

Result QueueQueryModel::refresh() {
  std::string queryStr =
      " select trk.title, art.name, alb.title, trk.trackNumber, alb.year, g.name, af.filePath, af.id from queue q"
      " join audioFiles af"
      " on q.audioFileId = af.id"
      " left outer join audioFile_to_playlists atp"
      " on af.id = atp.audioFileId"
      " join tracks trk"
      " on af.trackId = trk.id"
      " join albums alb"
      " on trk.albumId = alb.id"
      " join track_to_artists tta"
      " on tta.trackId = trk.id"
      " join artists art"
      " on tta.artistId = art.id"
      " left outer join track_to_genres ttg"
      " on trk.id = ttg.trackId"
      " left outer join genres g"
      " on g.id = ttg.genreId"
      " where true";

  if (filters->filters.at(ALBUM).ids.size() > 0)
    queryStr += " and alb.id in " + filters->idSqlArray(ALBUM);

  if (filters->filters.at(ARTIST).ids.size() > 0)
    queryStr += " and art.id in " + filters->idSqlArray(ARTIST);

  if (filters->filters.at(GENRE).ids.size() > 0)
    queryStr += " and g.id in " + filters->idSqlArray(GENRE);

  if (filters->filters.at(PLAYLIST).ids.size() > 0)
    queryStr += " and atp.playlistId in " + filters->idSqlArray(PLAYLIST);

  queryStr += " order by q.trackNumber asc";

  setQueryString(queryStr);
  setHeaderData(AUDIO_FILE_COL_TRACK, Qt::Horizontal, QObject::tr("Track"));
  setHeaderData(AUDIO_FILE_COL_ARTIST, Qt::Horizontal, QObject::tr("Artist"));
  setHeaderData(AUDIO_FILE_COL_ALBUM, Qt::Horizontal, QObject::tr("Album"));
  setHeaderData(AUDIO_FILE_COL_TRACK_NUMBER, Qt::Horizontal, QObject::tr("Track Number"));
  setHeaderData(AUDIO_FILE_COL_YEAR, Qt::Horizontal, QObject::tr("Year"));
  setHeaderData(AUDIO_FILE_COL_GENRE, Qt::Horizontal, QObject::tr("Genre"));
  setHeaderData(AUDIO_FILE_COL_PATH, Qt::Horizontal, QObject::tr("Path"));
  setHeaderData(AUDIO_FILE_COL_ID, Qt::Horizontal, QObject::tr("Id"));
  return OK;
}


} // Gui
} // Gj