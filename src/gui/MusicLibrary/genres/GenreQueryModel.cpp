//
// Created by ns on 10/11/25.
//

#include "GenreQueryModel.h"

namespace Gj {
namespace Gui {

Result GenreQueryModel::hydrateState(const AppStatePacket& appStatePacket) {
  // TODO
  // - if newQuery == query and refresh == false
  //   - no update
  // - else
  //   - setQuery(newQuery)
  //   - update()

  Logging::write(
      Info,
      "Gui::GenreQueryModel::hydrateState",
      "Setting Genre Query: ===== newQuery ====="
  );
  return OK;
}

QVariant GenreQueryModel::data(const QModelIndex& index, int role) const {
  // TODO: format
  return QSqlQueryModel::data(index, role);
}

} // Gui
} // Gj