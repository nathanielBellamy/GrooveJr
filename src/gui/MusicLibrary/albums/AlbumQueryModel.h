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
#include "../MusicLibraryFilters.h"

namespace Gj {
namespace Gui {

class AlbumQueryModel final : public QSqlQueryModel {
  MusicLibraryFilters* filters;
  QString query = QString();


public:
  Result refresh();
  explicit AlbumQueryModel(QObject* parent, MusicLibraryFilters* filters)
    : QSqlQueryModel(parent)
    , filters(filters)
    {
    refresh();
  }
  Result hydrateState(const AppStatePacket& appStatePacket);
  QVariant data(const QModelIndex &item, int role) const override;
};

} // Gui
} // Gj

#endif //ALBUMQUERYMODEL_H
