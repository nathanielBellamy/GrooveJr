//
// Created by ns on 11/16/25.
//

#include "ScenesQueryModel.h"

namespace Gj {
namespace Gui {

bool ScenesQueryModel::isCurrentScene(const QModelIndex& item) const {
  const Db::ID id = index(item.row(), SCENES_COL_ID).data().toULongLong();
  return gAppState->getScene().id == id;
}

QVariant ScenesQueryModel::data(const QModelIndex& item, int role) const {
  if (role == Qt::BackgroundRole && isCurrentScene(item))
    return QVariant::fromValue(QColor(CURRENT_SCENE_COLOR));

  return QStandardItemModel::data(item, role);
}

Result ScenesQueryModel::hydrateState(const AppStatePacket& appStatePacket) {
  refresh();
  return OK;
}

Result ScenesQueryModel::refresh(const bool hard) {
  std::string queryStr =
      " select sc.name, sc.id from scenes sc";


  if (hard || queryHasChanged(queryStr.c_str())) {
    emit runQuery(id, QString(queryStr.c_str()));
    setPreviousQuery(queryStr);
  }
  return OK;
}

Result ScenesQueryModel::setHeaders() {
  setHeaderData(SCENES_COL_ID, Qt::Horizontal, QObject::tr("Id"));
  setHeaderData(SCENES_COL_NAME, Qt::Horizontal, QObject::tr("Name"));
  return OK;
}

} // Gui
} // Gj