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

QVariant ArtistQueryModel::data(const QModelIndex& index, int role) const {
  // TODO: format
  return QSqlQueryModel::data(index, role);
}

Result ArtistQueryModel::refresh() {
  std::string queryStr = " select art.name, art.id from artists art";
  // todo:
  // switch (filters->type) {
  //   case ALBUM:
  //     queryStr = " select art.name, art.id from artists art"
  //                " join artist_to_albums ata"
  //                " on art.id = ata.artistId"
  //                " where ata.albumId in " + filters->idSqlArray();
  //     break;
  //   case AUDIO_FILE:
  //     queryStr = " select art.name, art.id from artists art"
  //                " join track_to_artists tta"
  //                " on art.id = tta.artistId"
  //                " join audioFiles af"
  //                " on af.trackId = tta.trackId"
  //                " where af.id in " + filters->idSqlArray() +
  //                " group by art.id";
  //     break;
  //   case GENRE:
  //     queryStr = " select art.name, art.id from artists art"
  //                " join track_to_artists tta"
  //                " on art.id = tta.artistId"
  //                " join track_to_genres ttg"
  //                " on tta.trackId = ttg.trackId"
  //                " where ttg.genreId in " + filters->idSqlArray() +
  //                " group by art.id";
  //     break;
  //   case PLAYLIST:
  //     queryStr = " select art.name, art.id from artists art"
  //                " join track_to_artists tta"
  //                " on tta.artistId = art.id"
  //                " join audioFiles af"
  //                " on tta.trackId = af.trackId"
  //                " join audioFile_to_playlists atp"
  //                " on af.id = atp.audioFileId"
  //                " where atp.playlistId in " + filters->idSqlArray();
  //     break;
  //   case TRACK:
  //     queryStr = " select art.name, art.id from artists art"
  //                " join track_to_artists tta"
  //                " on art.id = tta.artistId"
  //                " where tta.trackId in " + filters->idSqlArray() +
  //                " group by art.id";
  //     break;
  //   default:
  //     queryStr = "select name, id from artists";
  // }

  setQueryString(queryStr);
  setHeaderData(ARTIST_COL_NAME, Qt::Horizontal, QObject::tr("Name"));
  setHeaderData(ARTIST_COL_ID, Qt::Horizontal, QObject::tr("Id"));
  return OK;
}

} // Gui
} // Gj