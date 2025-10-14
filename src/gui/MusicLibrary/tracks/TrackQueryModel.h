//
// Created by ns on 10/11/25.
//

#ifndef TRACKQUERYMODEL_H
#define TRACKQUERYMODEL_H

#include <QtSql/qsqlquerymodel.h>
#include <QVariant>
#include <QColor>
#include <QString>

#include "../../../AppState.h"
#include "../../../Logging.h"
#include "../../../enums/Result.h"

namespace Gj {
namespace Gui {

class TrackQueryModel final : public QSqlQueryModel {
  QString query = QString("select title, trackNumber from tracks");

public:
  explicit TrackQueryModel(QObject* parent = nullptr)
    : QSqlQueryModel(parent)
    {

    setQuery(query);
    setHeaderData(0, Qt::Horizontal, QObject::tr("Title"));
    setHeaderData(1, Qt::Horizontal, QObject::tr("TrackNumber"));
  }
  Result hydrateState(const AppStatePacket& appStatePacket);

  QVariant data(const QModelIndex& index, int role) const override;
};

} // Gui
} // Gj

#endif //TRACKQUERYMODEL_H
