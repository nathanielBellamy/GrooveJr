//
// Created by ns on 10/11/25.
//

#ifndef ALBUMQUERYMODEL_H
#define ALBUMQUERYMODEL_H

#include <QtSql/qsqlquerymodel.h>
#include <QVariant>
#include <QColor>
#include <QString>

#include "../../../AppState.h"
#include "../../../Logging.h"
#include "../../../enums/Result.h"

namespace Gj {
namespace Gui {

class AlbumQueryModel final : public QSqlQueryModel {
  QString query = QString("select title, year from albums");

  Qt::ItemFlags flags(const QModelIndex &index) const override;

  Result refresh() {
    setQuery(query);
    setHeaderData(0, Qt::Horizontal, QObject::tr("Title"));
    setHeaderData(1, Qt::Horizontal, QObject::tr("Year"));

    return OK;
  }

public:
  explicit AlbumQueryModel(QObject* parent = nullptr)
    : QSqlQueryModel(parent)
    {

    refresh();
  }
  Result hydrateState(const AppStatePacket& appStatePacket);
  QVariant data(const QModelIndex &item, int role) const override;
};

} // Gui
} // Gj

#endif //ALBUMQUERYMODEL_H
