//
// Created by ns on 11/16/25.
//

#include "ScenesQueryModel.h"

namespace Gj {
namespace Gui {

bool ScenesQueryModel::isCurrentScene(const QModelIndex& item) const {
  const ID id = index(item.row(), SCENES_COL_ID).data().toULongLong();
  return stateCore->getScene().id == id;
}

QVariant ScenesQueryModel::data(const QModelIndex& item, int role) const {
  if (role == Qt::BackgroundRole && isCurrentScene(item))
    return QVariant::fromValue(QColor(CURRENT_SCENE_COLOR));

  return QStandardItemModel::data(item, role);
}

Result ScenesQueryModel::hydrateState(const State::Packet& statePacket) {
  refresh();
  return OK;
}

Result ScenesQueryModel::refresh(const bool hard) {
  std::string queryStr =
      " select sc.sceneId, sc.name, MAX(sc.version), sc.id from scenes sc"
      " group by sc.sceneId"
      " order by sc.sceneId asc";

  if (hard || queryHasChanged(queryStr.c_str())) {
    emit runQuery(id, QString(queryStr.c_str()));
    setPreviousQuery(queryStr);
  }
  return OK;
}

Result ScenesQueryModel::setHeaders() {
  setHeaderData(SCENES_COL_ID, Qt::Horizontal, QObject::tr("_Id"));
  setHeaderData(SCENES_COL_SCENE_ID, Qt::Horizontal, QObject::tr("Id"));
  setHeaderData(SCENES_COL_VERSION, Qt::Horizontal, QObject::tr("Version"));
  setHeaderData(SCENES_COL_NAME, Qt::Horizontal, QObject::tr("Name"));
  return OK;
}

} // Gui
} // Gj