//
// Created by ns on 10/11/25.
//

#include "TrackQueryModel.h"

namespace Gj {
namespace Gui {

Result TrackQueryModel::hydrateState(const AppStatePacket& appStatePacket) {
  // TODO
  // - if newQuery == query and refresh == false
  //   - no update
  // - else
  //   - setQuery(newQuery)
  //   - update()

  Logging::write(
      Info,
      "Gui::TrackQueryModel::hydrateState",
      "Setting Track Query: ===== newQuery ====="
  );
  return OK;
}

QVariant TrackQueryModel::data(const QModelIndex& index, const int role) const {
  // TODO: format
  // if (role == Qt::ForegroundRole)
  //   return QVariant::fromValue(QColor(Qt::green));

  return QSqlQueryModel::data(index, role);
}


Result TrackQueryModel::refresh() {
  std::string queryStr;
  switch(filters->filterBy) {
    case ALBUM:
      queryStr = " select title, trackNumber, id from tracks"
                 " where albumId in "
                 + filters->idSqlArray()
                 + " order by trackNumber asc";
      break;
    case ARTIST:
      queryStr = " select title, trackNumber, id from tracks t"
                 " join track_to_artists tta"
                 " on t.id = tta.trackId"
                 " where tta.artistId in " + filters->idSqlArray();
      break;
    default:
      queryStr = "select title, trackNumber, id from tracks";
  }

  setQuery(QString(queryStr.c_str()));
  setHeaderData(0, Qt::Horizontal, QObject::tr("Title"));
  setHeaderData(1, Qt::Horizontal, QObject::tr("TrackNumber"));
  setHeaderData(2, Qt::Horizontal, QObject::tr("Id"));

  return OK;
}

} // Gui
} // Gj