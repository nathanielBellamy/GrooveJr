//
// Created by ns on 10/11/25.
//

#include "AudioFileQueryModel.h"

namespace Gj {
namespace Gui {

Result AudioFileQueryModel::hydrateState(const AppStatePacket& appStatePacket) {
  Logging::write(
      Info,
      "Gui::AudioFileQueryModel::hydrateState",
      "AudioFileQueryModel::hydrateState"
  );
  return OK;
}

QVariant AudioFileQueryModel::data(const QModelIndex& index, int role) const {
  // TODO: format
  return QSqlQueryModel::data(index, role);
}

Result AudioFileQueryModel::refresh() {
  std::string queryStr;
  switch (filters->type) {
    case ALBUM:
      queryStr = " select trk.title, art.name, alb.title, trk.trackNumber, alb.year, af.filePath, af.id from audioFiles af"
                 " join tracks trk"
                 " on af.trackId = trk.id"
                 " join albums alb"
                 " on trk.albumId = alb.id"
                 " join track_to_artists tta"
                 " on tta.trackId = trk.id"
                 " join artists art"
                 " on tta.artistId = art.id"
                 " where alb.id in " + filters->idSqlArray() +
                 " order by trk.trackNumber asc, af.filePath asc";
      break;
    case ARTIST:
      queryStr = " select trk.title, art.name, alb.title, trk.trackNumber, alb.year, af.filePath, af.id from audioFiles af"
                 " join tracks trk"
                 " on af.trackId = trk.id"
                 " join albums alb"
                 " on trk.albumId = alb.id"
                 " join track_to_artists tta"
                 " on tta.trackId = trk.id"
                 " join artists art"
                 " on tta.artistId = art.id"
                 " where art.id in " + filters->idSqlArray() +
                 " order by alb.year asc, alb.title asc, alb.id asc, trk.trackNumber asc";
      break;
    case GENRE:
      queryStr = " select trk.title, art.name, alb.title, trk.trackNumber, alb.year, af.filePath, af.id from audioFiles af"
                 " join tracks trk"
                 " on af.trackId = trk.id"
                 " join albums alb"
                 " on trk.albumId = alb.id"
                 " join track_to_artists tta"
                 " on tta.trackId = trk.id"
                 " join artists art"
                 " on tta.artistId = art.id"
                 " join track_to_genres ttg"
                 " on trk.id = ttg.trackId"
                 " where ttg.genreId in " + filters->idSqlArray() +
                 " order by art.name asc, alb.year asc, alb.title asc, trk.trackNumber asc";
      break;
    case PLAYLIST:
      queryStr = " select trk.title, art.name, alb.title, trk.trackNumber, alb.year, af.filePath, af.id from audioFiles af"
                 " join tracks trk"
                 " on af.trackId = trk.id"
                 " join albums alb"
                 " on trk.albumId = alb.id"
                 " join track_to_artists tta"
                 " on tta.trackId = trk.id"
                 " join artists art"
                 " on tta.artistId = art.id"
                 " join audioFile_to_playlists atp"
                 " on af.id = atp.audioFileId"
                 " where atp.playlistId in " + filters->idSqlArray() +
                 " order by atp.trackNumber asc";
      break;
    case TRACK:
      queryStr = " select trk.title, art.name, alb.title, trk.trackNumber, alb.year, af.filePath, af.id from audioFiles af"
                 " join tracks trk"
                 " on af.trackId = trk.id"
                 " join albums alb"
                 " on trk.albumId = alb.id"
                 " join track_to_artists tta"
                 " on tta.trackId = trk.id"
                 " join artists art"
                 " on tta.artistId = art.id"
                 " where trk.id in " + filters->idSqlArray() +
                 " order by af.filePath asc";
      break;
    default:
      queryStr = " select trk.title, art.name, alb.title, trk.trackNumber, alb.year, af.filePath, af.id from audioFiles af"
                 " join tracks trk"
                 " on af.trackId = trk.id"
                 " join albums alb"
                 " on trk.albumId = alb.id"
                 " join track_to_artists tta"
                 " on tta.trackId = trk.id"
                 " join artists art"
                 " on tta.artistId = art.id"
                 " order by art.name asc, alb.year asc, alb.title asc, trk.trackNumber asc";
  }

  query = QString(queryStr.c_str());
  setQuery(query);
  setHeaderData(0, Qt::Horizontal, QObject::tr("Track"));
  setHeaderData(1, Qt::Horizontal, QObject::tr("Artist"));
  setHeaderData(2, Qt::Horizontal, QObject::tr("Album"));
  setHeaderData(3, Qt::Horizontal, QObject::tr("Track Number"));
  setHeaderData(4, Qt::Horizontal, QObject::tr("Year"));
  setHeaderData(5, Qt::Horizontal, QObject::tr("Path"));
  setHeaderData(6, Qt::Horizontal, QObject::tr("Id"));

  return OK;
}


} // Gui
} // Gj