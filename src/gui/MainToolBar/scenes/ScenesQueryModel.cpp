//
// Created by ns on 11/16/25.
//

#include "ScenesQueryModel.h"

namespace Gj {
namespace Gui {


QVariant ScenesQueryModel::data(const QModelIndex& item, int role) const {

  return OK;
}

Result ScenesQueryModel::hydrateState(const AppStatePacket& appStatePacket) {

  return OK;
}

Result ScenesQueryModel::refresh() {

  return OK;
}

Result ScenesQueryModel::setHeaders() {

  return OK;
}

} // Gui
} // Gj