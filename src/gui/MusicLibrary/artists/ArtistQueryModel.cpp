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
  if (!filters->albumIds.empty()) {
    const std::string queryStr = " select name from artists art"
                                 " join artist_to_albums ata"
                                 " on art.id = ata.artistId"
                                 " where ata.albumId = " + std::to_string(filters->albumIds.front());
    query = QString(queryStr.c_str());
  } else {
    query = QString("select name from artists");
  }

  setQuery(query);
  setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));

  return OK;
}

} // Gui
} // Gj