//
// Created by ns on 10/13/25.
//

#include "PlaylistQueryModel.h"

namespace Gj {
namespace Gui {

Result PlaylistQueryModel::hydrateState(const AppStatePacket& appStatePacket) {
  // TODO
  // - if newQuery == query and refresh == false
  //   - no update
  // - else
  //   - setQuery(newQuery)
  //   - update()

  Logging::write(
      Info,
      "Gui::PlaylistQueryModel::hydrateState",
      "Setting Playlist Query: ===== newQuery ====="
  );
  return OK;
}

QVariant PlaylistQueryModel::data(const QModelIndex& index, int role) const {
  // TODO: format
  return QSqlQueryModel::data(index, role);
}

} // Gui
} // Gj