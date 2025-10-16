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
  switch (filters->filterBy) {
    case ALBUM:
      queryStr = " select af.filePath, af.id from audioFiles af"
                 " join tracks trk"
                 " on af.trackId = trk.id"
                 " where trk.albumId in " + filters->idSqlArray() +
                 " group by af.id";
      break;
    case ARTIST:
      queryStr = " select af.filePath, af.id from audioFiles af"
                 " join tracks trk"
                 " on af.trackId = trk.id"
                 " join track_to_artists tta"
                 " on trk.id = tta.trackId"
                 " where tta.artistId in " + filters->idSqlArray();
      break;
    case GENRE:
      queryStr = " select af.filePath, af.id from audioFiles af"
                 " join tracks trk"
                 " on af.trackId = trk.id"
                 " join track_to_genres ttg"
                 " on trk.id = ttg.trackId"
                 " where ttg.genreId in " + filters->idSqlArray();
      break;
    case PLAYLIST:
      queryStr = " select af.filePath, af.id from audioFiles af"
                 " join audioFile_to_playlists atp"
                 " on af.id = atp.audioFileId"
                 " where atp.playlistId in " + filters->idSqlArray();
      break;
    case TRACK:
      queryStr = " select filePath, id from audioFiles"
                 " where audioFiles.trackId in " + filters->idSqlArray();
      break;
    default:
      queryStr = "select filePath, id from audioFiles";
  }

  query = QString(queryStr.c_str());
  setQuery(query);
  setHeaderData(0, Qt::Horizontal, QObject::tr("Path"));
  setHeaderData(1, Qt::Horizontal, QObject::tr("Id"));

  return OK;
}


} // Gui
} // Gj