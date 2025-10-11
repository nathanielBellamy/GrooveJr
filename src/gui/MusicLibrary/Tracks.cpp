//
// Created by ns on 10/11/25.
//

#include "Tracks.h"

namespace Gj {
namespace Gui {

QVariant Tracks::data(const QModelIndex &index, int role) const {
  QVariant value = QSqlQueryModel::data(index, role);
  if (value.isValid() && role == Qt::DisplayRole) {
    if (index.column() == 0)
      return value.toString().prepend('#');
    else if (index.column() == 2)
      return value.toString().toUpper();
  }
  if (role == Qt::ForegroundRole && index.column() == 1)
    return QVariant::fromValue(QColor(Qt::blue));
  return value;
}

} // Gui
} // Gj