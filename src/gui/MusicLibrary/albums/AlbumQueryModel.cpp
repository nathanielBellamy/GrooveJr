//
// Created by ns on 10/11/25.
//

#include "AlbumQueryModel.h"

namespace Gj {
namespace Gui {

Result AlbumQueryModel::hydrateState(const AppStatePacket& appStatePacket) {
  Logging::write(
      Info,
      "Gui::AlbumQueryModel::hydrateState",
      "AlbumQueryModel::hydrateState"
  );
  return OK;
}

Result AlbumQueryModel::refresh(const bool hard) {
  std::string queryStr =
    " select alb.title, alb.year, alb.id from albums alb"
    " left outer join artist_to_albums ata"
    " on alb.id = ata.albumId"
    " join tracks trk"
    " on alb.id = trk.albumId"
    " left outer join track_to_genres ttg"
    " on trk.id = ttg.trackId"
    " join audioFiles af"
    " on trk.id = af.trackId"
    " left outer join audioFile_to_playlists atp"
    " on af.id = atp.audioFileId"
    " where true";

  if (filters->filters.at(ARTIST).ids.size() > 0)
    queryStr += " and ata.artistId in " + filters->idSqlArray(ARTIST);

  if (filters->filters.at(GENRE).ids.size() > 0)
    queryStr += " and ttg.genreId in " + filters->idSqlArray(GENRE);

  if (filters->filters.at(PLAYLIST).ids.size() > 0)
    queryStr += " and atp.playlistId in " + filters->idSqlArray(PLAYLIST);

  queryStr +=
    " group by alb.id"
    " order by alb.title, alb.year";

  if (hard || queryHasChanged(queryStr.c_str())) {
    emit runQuery(id, QString(queryStr.c_str()));
    setPreviousQuery(queryStr);
  }
  return OK;
}

Result AlbumQueryModel::setHeaders() {
  setHeaderData(ALBUM_COL_TITLE, Qt::Horizontal, QObject::tr("Title"));
  setHeaderData(ALBUM_COL_YEAR, Qt::Horizontal, QObject::tr("Year"));
  setHeaderData(ALBUM_COL_ID, Qt::Horizontal, QObject::tr("Id"));
  return OK;
};

QVariant AlbumQueryModel::data(const QModelIndex& item, const int role) const {
  if (const QVariant parentData = MusicLibraryQueryModel::data(item, role); !parentData.isNull())
    return parentData;

  return QStandardItemModel::data(item, role);
}

bool AlbumQueryModel::isCurrentlyPlaying(const QModelIndex& item) const {
  const ID id = index(item.row(), ALBUM_COL_ID).data().toULongLong();
  return gAppState->getCurrentlyPlaying().album.id == id;
}

} // Gui
} // Gj