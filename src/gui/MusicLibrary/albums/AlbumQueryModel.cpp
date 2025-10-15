//
// Created by ns on 10/11/25.
//

#include "AlbumQueryModel.h"

namespace Gj {
namespace Gui {

Result AlbumQueryModel::hydrateState(const AppStatePacket& appStatePacket) {
  // TODO
  // - if newQuery == query and refresh == false
  //   - no update
  // - else
  //   - setQuery(newQuery)
  //   - update()
  // refresh();

  Logging::write(
      Info,
      "Gui::AlbumQueryModel::hydrateState",
      "Setting Album Query: ===== newQuery ====="
  );
  return OK;
}

Result AlbumQueryModel::refresh() {
  std::string queryStr;
  switch (filters->filterBy) {
    case ARTIST:
      queryStr = " select title, year, id from albums alb"
                 " join artist_to_albums ata"
                 " on alb.id = ata.albumId"
                 " where ata.artistId in " + filters->idSqlArray();
      break;
    case TRACK:
      queryStr = " select alb.title, alb.year, alb.id from albums alb"
                 " join tracks trk"
                 " on alb.id = trk.albumId"
                 " where trk.id in " + filters->idSqlArray();
      break;
    default:
      queryStr = "select title, year, id from albums";
  }

  query = QString(queryStr.c_str());
  setQuery(query);
  setHeaderData(0, Qt::Horizontal, QObject::tr("Title"));
  setHeaderData(1, Qt::Horizontal, QObject::tr("Year"));
  setHeaderData(2, Qt::Horizontal, QObject::tr("Id"));

  return OK;
}

QVariant AlbumQueryModel::data(const QModelIndex& index, int role) const {
  // TODO: format
  return QSqlQueryModel::data(index, role);
}

} // Gui
} // Gj