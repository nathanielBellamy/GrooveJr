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
  std::string queryStr = " select * from queue";
  // switch (filters->type) {
  //   case ALBUM:
  //     queryStr = " select trk.title, art.name, alb.title, trk.trackNumber, alb.year, af.filePath, q.id from queue q"
  //                " join audioFiles af"
  //                " on af.id = q.audioFileId"
  //                " join tracks trk"
  //                " on af.trackId = trk.id"
  //                " join albums alb"
  //                " on trk.albumId = alb.id"
  //                " join track_to_artists tta"
  //                " on tta.trackId = trk.id"
  //                " join artists art"
  //                " on tta.artistId = art.id"
  //                " where alb.id in " + filters->idSqlArray() +
  //                " order by q.trackNumber asc";
  //     break;
  //   case ARTIST:
  //     queryStr = " select trk.title, art.name, alb.title, trk.trackNumber, alb.year, af.filePath, q.id from queue q"
  //                " join audioFiles af"
  //                " on af.id = q.audioFileId"
  //                " join tracks trk"
  //                " on af.trackId = trk.id"
  //                " join albums alb"
  //                " on trk.albumId = alb.id"
  //                " join track_to_artists tta"
  //                " on tta.trackId = trk.id"
  //                " join artists art"
  //                " on tta.artistId = art.id"
  //                " where art.id in " + filters->idSqlArray() +
  //                " order by q.trackNumber asc";
  //     break;
  //   case GENRE:
  //     queryStr = " select trk.title, art.name, alb.title, trk.trackNumber, alb.year, af.filePath, q.id from queue q"
  //                " join audioFiles af"
  //                " on af.id = q.audioFileId"
  //                " join tracks trk"
  //                " on af.trackId = trk.id"
  //                " join albums alb"
  //                " on trk.albumId = alb.id"
  //                " join track_to_artists tta"
  //                " on tta.trackId = trk.id"
  //                " join artists art"
  //                " on tta.artistId = art.id"
  //                " join track_to_genres ttg"
  //                " on trk.id = ttg.trackId"
  //                " where ttg.genreId in " + filters->idSqlArray() +
  //                " order by q.trackNumber asc";
  //     break;
  //   case PLAYLIST:
  //     queryStr = " select trk.title, art.name, alb.title, trk.trackNumber, alb.year, af.filePath, q.id from queue q"
  //                " join audioFiles af"
  //                " on af.id = q.audioFileId"
  //                " join tracks trk"
  //                " on af.trackId = trk.id"
  //                " join albums alb"
  //                " on trk.albumId = alb.id"
  //                " join track_to_artists tta"
  //                " on tta.trackId = trk.id"
  //                " join artists art"
  //                " on tta.artistId = art.id"
  //                " join audioFile_to_playlists atp"
  //                " on af.id = atp.audioFileId"
  //                " where atp.playlistId in " + filters->idSqlArray() +
  //                " order by q.trackNumber asc";
  //     break;
  //   case TRACK:
  //     queryStr = " select trk.title, art.name, alb.title, trk.trackNumber, alb.year, af.filePath, q.id from queue q"
  //                " join audioFiles af"
  //                " on af.id = q.audioFileId"
  //                " join tracks trk"
  //                " on af.trackId = trk.id"
  //                " join albums alb"
  //                " on trk.albumId = alb.id"
  //                " join track_to_artists tta"
  //                " on tta.trackId = trk.id"
  //                " join artists art"
  //                " on tta.artistId = art.id"
  //                " where trk.id in " + filters->idSqlArray() +
  //                " order by q.trackNumber asc";
  //     break;
  //   default:
  //     queryStr = " select trk.title, art.name, alb.title, trk.trackNumber, alb.year, af.filePath, q.id from queue q"
  //                " join audioFiles af"
  //                " on af.id = q.audioFileId"
  //                " join tracks trk"
  //                " on af.trackId = trk.id"
  //                " join albums alb"
  //                " on trk.albumId = alb.id"
  //                " join track_to_artists tta"
  //                " on tta.trackId = trk.id"
  //                " join artists art"
  //                " on tta.artistId = art.id"
  //                " order by q.trackNumber asc";
  // }

  setQueryString(queryStr);
  setHeaderData(AUDIO_FILE_COL_TRACK, Qt::Horizontal, QObject::tr("Track"));
  setHeaderData(AUDIO_FILE_COL_ARTIST, Qt::Horizontal, QObject::tr("Artist"));
  setHeaderData(AUDIO_FILE_COL_ALBUM, Qt::Horizontal, QObject::tr("Album"));
  setHeaderData(AUDIO_FILE_COL_TRACK_NUMBER, Qt::Horizontal, QObject::tr("Track Number"));
  setHeaderData(AUDIO_FILE_COL_YEAR, Qt::Horizontal, QObject::tr("Year"));
  setHeaderData(AUDIO_FILE_COL_PATH, Qt::Horizontal, QObject::tr("Path"));
  setHeaderData(AUDIO_FILE_COL_ID, Qt::Horizontal, QObject::tr("Id"));

  return OK;
}


} // Gui
} // Gj