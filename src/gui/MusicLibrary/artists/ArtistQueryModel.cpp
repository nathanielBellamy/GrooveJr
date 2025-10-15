//
// Created by ns on 10/11/25.
//

#include "ArtistQueryModel.h"

namespace Gj {
namespace Gui {

Result ArtistQueryModel::hydrateState(const AppStatePacket& appStatePacket) {
  // TODO
  // - if newQuery == query and refresh == false
  //   - no update
  // - else
  //   - setQuery(newQuery)
  //   - update()

  Logging::write(
      Info,
      "Gui::ArtistQueryModel::hydrateState",
      "Setting Artist Query: ===== newQuery ====="
  );
  return OK;
}

QVariant ArtistQueryModel::data(const QModelIndex& index, int role) const {
  // TODO: format
  return QSqlQueryModel::data(index, role);
}

Result ArtistQueryModel::refresh() {
  if (!filters->albumIds.empty()) {
    const std::string queryStr = " select id, name from artists art"
                                 " join artist_to_albums ata"
                                 " on art.id = ata.artistId"
                                 " where ata.albumId = " + std::to_string(filters->albumIds.front());
    query = QString(queryStr.c_str());
  }

  setQuery(query);
  setHeaderData(0, Qt::Horizontal, QObject::tr("Id"));
  setHeaderData(1, Qt::Horizontal, QObject::tr("Title"));
  setHeaderData(2, Qt::Horizontal, QObject::tr("TrackNumber"));

  return OK;
}

} // Gui
} // Gj