//
// Created by ns on 10/11/25.
//

#include "TrackQueryModel.h"

namespace Gj {
namespace Gui {

Result TrackQueryModel::hydrateState(const AppStatePacket& appStatePacket) {
  Logging::write(
      Info,
      "Gui::TrackQueryModel::hydrateState",
      "TrackQueryModel::hydrateState"
  );
  return OK;
}

QVariant TrackQueryModel::data(const QModelIndex& item, const int role) const {
  if (role == Qt::BackgroundRole && isSelected(item, TRACK_COL_ID))
    return QVariant::fromValue(QColor(Qt::blue));
  return QSqlQueryModel::data(item, role);
}


Result TrackQueryModel::refresh() {
  std::string queryStr;
  switch(filters->filterBy) {
    case ALBUM:
      queryStr = " select t.title, t.trackNumber, t.id from tracks t"
                 " where t.albumId in " + filters->idSqlArray() +
                 " order by trackNumber asc";
      break;
    case ARTIST:
      queryStr = " select t.title, t.trackNumber, t.id from tracks t"
                 " join track_to_artists tta"
                 " on t.id = tta.trackId"
                 " where tta.artistId in " + filters->idSqlArray();
      break;
    case AUDIO_FILE:
      queryStr = " select t.title, t.trackNumber, t.id from tracks t"
                 " join audioFiles af"
                 " on t.id = af.trackId"
                 " where af.id in " + filters->idSqlArray() +
                 " group by t.id";
      break;
    case GENRE:
      queryStr = " select t.title, t.trackNumber, t.id from tracks t"
                 " join track_to_genres ttg"
                 " on ttg.trackId = t.id"
                 " where ttg.genreId in " + filters->idSqlArray() +
                 " group by t.id";
      break;
    case PLAYLIST:
      queryStr = " select trk.title, trk.trackNumber, trk.id from tracks trk"
                 " join audioFiles af"
                 " on trk.id = af.trackId"
                 " join audioFile_to_playlists atp"
                 " on af.id = atp.audioFileId"
                 " where atp.playlistId in " + filters->idSqlArray();
      break;
    default:
      queryStr = "select title, trackNumber, id from tracks";
  }

  setQueryString(queryStr);

  // TODO: TRACK_COL_XXX
  setHeaderData(0, Qt::Horizontal, QObject::tr("Title"));
  setHeaderData(1, Qt::Horizontal, QObject::tr("TrackNumber"));
  setHeaderData(2, Qt::Horizontal, QObject::tr("Id"));

  return OK;
}

} // Gui
} // Gj