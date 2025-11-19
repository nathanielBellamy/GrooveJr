//
// Created by ns on 10/11/25.
//

#include "ArtistQueryModel.h"

namespace Gj {
namespace Gui {

Result ArtistQueryModel::hydrateState(const AppStatePacket& appStatePacket) {
  Logging::write(
      Info,
      "Gui::ArtistQueryModel::hydrateState",
      "ArtistQueryModel::hydrateState"
  );
  return OK;
}

QVariant ArtistQueryModel::data(const QModelIndex& item, const int role) const {
  if (const QVariant parentData = MusicLibraryQueryModel::data(item, role); !parentData.isNull())
    return parentData;

  return QStandardItemModel::data(item, role);
}

Result ArtistQueryModel::refresh(const bool hard) {
  std::string queryStr =
    " select art.name, art.id from artists art"
    " join artist_to_albums ata"
    " on art.id = ata.artistId"
    " left outer join track_to_artists tta"
    " on art.id = tta.artistId"
    " left outer join track_to_genres ttg"
    " on tta.trackId = ttg.trackId"
    " left outer join audioFiles af"
    " on af.trackId = tta.trackId"
    " left outer join audioFile_to_playlists atp"
    " on af.id = atp.audioFileId"
    " where true";

  if (filters->filters.at(ALBUM).ids.size() > 0)
    queryStr += " and ata.albumId in " + filters->idSqlArray(ALBUM);

  if (filters->filters.at(GENRE).ids.size() > 0)
    queryStr += " and ttg.genreId in " + filters->idSqlArray(GENRE);

  if (filters->filters.at(PLAYLIST).ids.size() > 0)
    queryStr += " and atp.playlistId in " + filters->idSqlArray(PLAYLIST);

  queryStr +=
    " group by art.id"
    " order by art.name";

  if (hard || queryHasChanged(queryStr.c_str())) {
    emit runQuery(id, QString(queryStr.c_str()));
    setPreviousQuery(queryStr);
  }
  return OK;
}


Result ArtistQueryModel::setHeaders() {
  setHeaderData(ARTIST_COL_NAME, Qt::Horizontal, QObject::tr("Name"));
  setHeaderData(ARTIST_COL_ID, Qt::Horizontal, QObject::tr("Id"));
  return OK;
}

bool ArtistQueryModel::isCurrentlyPlaying(const QModelIndex& item) const {
  const Db::ID id = index(item.row(), ARTIST_COL_ID).data().toULongLong();
  return gAppState->getCurrentlyPlaying().artist.id == id;
}

} // Gui
} // Gj