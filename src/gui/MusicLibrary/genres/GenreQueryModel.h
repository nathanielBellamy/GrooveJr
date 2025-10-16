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
#include "../MusicLibraryFilters.h"

namespace Gj {
namespace Gui {

class GenreQueryModel final : public QSqlQueryModel {
  MusicLibraryFilters* filters;
  QString query;

public:
  explicit GenreQueryModel(QObject* parent, MusicLibraryFilters* filters)
    : QSqlQueryModel(parent)
    , filters(filters)
    {

    setQuery(query);
    setHeaderData(0, Qt::Horizontal, QObject::tr("Name"));
  }
  Result hydrateState(const AppStatePacket& appStatePacket);
  Result refresh();

  QVariant data(const QModelIndex &item, int role) const override;
};

} // Gui
} // Gj

#endif //GENREQUERYMODEL_H
