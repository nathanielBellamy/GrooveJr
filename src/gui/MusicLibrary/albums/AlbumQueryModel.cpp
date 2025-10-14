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

QVariant AlbumQueryModel::data(const QModelIndex& index, int role) const {
  // TODO: format
  return QSqlQueryModel::data(index, role);
}

Qt::ItemFlags AlbumQueryModel::flags(const QModelIndex &index) const {
  Qt::ItemFlags flags = QSqlQueryModel::flags(index);
  flags |= Qt::ItemIsEditable;
  return flags;
}

} // Gui
} // Gj