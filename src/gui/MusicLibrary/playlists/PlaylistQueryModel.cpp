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

QVariant PlaylistQueryModel::data(const QModelIndex& item, const int role) const {
  if (const QVariant parentData = MusicLibraryQueryModel::data(item, role); !parentData.isNull())
    return parentData;

  return QStandardItemModel::data(item, role);
}

Result PlaylistQueryModel::refresh(const bool hard) {
  std::string queryStr =
    " select p.name, p.id from playlists p"
    " left outer join audioFile_to_playlists atp"
    " on p.id = atp.playlistId"
    " left outer join audioFiles af"
    " on af.id = atp.audioFileId"
    " left outer join tracks trk"
    " on trk.id = af.trackId"
    " left outer join track_to_artists tta"
    " on trk.id = tta.trackId"
    " left outer join track_to_genres ttg"
    " on trk.id = ttg.trackId"
    " where true";

  if (filters->filters.at(ALBUM).ids.size() > 0)
    queryStr += " and trk.albumId in " + filters->idSqlArray(ALBUM);

  if (filters->filters.at(ARTIST).ids.size() > 0)
    queryStr += " and tta.artistId in " + filters->idSqlArray(ARTIST);

  if (filters->filters.at(GENRE).ids.size() > 0)
    queryStr += " and ttg.genreId in " + filters->idSqlArray(GENRE);

  queryStr +=
    " group by p.id"
    " order by p.name";

  if (hard || queryHasChanged(queryStr.c_str())) {
    emit runQuery(id, QString(queryStr.c_str()));
    setPreviousQuery(queryStr);
  }

  return OK;
}

Result PlaylistQueryModel::setHeaders() {
  setHeaderData(PLAYLIST_COL_NAME, Qt::Horizontal, QObject::tr("Name"));
  setHeaderData(PLAYLIST_COL_ID, Qt::Horizontal, QObject::tr("Id"));
  return OK;
}

bool PlaylistQueryModel::isCurrentlyPlaying(const QModelIndex& item) const {
  const Db::ID id = index(item.row(), AUDIO_FILE_COL_ID).data().toULongLong();
  return false; // TODO
};

} // Gui
} // Gj