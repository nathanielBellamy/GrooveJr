//
// Created by ns on 10/11/25.
//

#ifndef GENREQUERYMODEL_H
#define GENREQUERYMODEL_H

#include <QtSql/qsqlquerymodel.h>
#include <QVariant>
#include <QColor>
#include <QString>

#include "../../../AppState.h"
#include "../../../Logging.h"
#include "../../../enums/Result.h"

namespace Gj {
namespace Gui {

class GenreQueryModel final : public QSqlQueryModel {
  QString query = QString("select name from genres");

public:
  explicit GenreQueryModel(QObject* parent = nullptr)
    : QSqlQueryModel(parent)
    {

    setQuery(query);
    setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
  }
  Result hydrateState(const AppStatePacket& appStatePacket);

  QVariant data(const QModelIndex &item, int role) const override;
};

} // Gui
} // Gj

#endif //GENREQUERYMODEL_H
