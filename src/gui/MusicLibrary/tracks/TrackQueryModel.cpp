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

QVariant TrackQueryModel::data(const QModelIndex& index, int role) const {
  // TODO: format
  return QSqlQueryModel::data(index, role);
}

} // Gui
} // Gj