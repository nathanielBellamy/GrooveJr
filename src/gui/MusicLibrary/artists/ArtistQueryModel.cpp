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
      "Hydrating state to AQM"
  );
  return OK;
}

QVariant ArtistQueryModel::data(const QModelIndex& index, int role) const {
  // TODO: format
  return QSqlQueryModel::data(index, role);
}

Result ArtistQueryModel::refresh() {
  std::string queryStr;
  switch (filters->filterBy) {
    case ALBUM:
      queryStr = " select name, id from artists art"
                 " join artist_to_albums ata"
                 " on art.id = ata.artistId"
                 " where ata.albumId in " + filters->idSqlArray();
      break;
    default:
      queryStr = "select name, id from artists";
  }

  setQuery(QString(queryStr.c_str()));
  setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
  setHeaderData(1, Qt::Horizontal, QObject::tr("Id"));
  return OK;
}

} // Gui
} // Gj