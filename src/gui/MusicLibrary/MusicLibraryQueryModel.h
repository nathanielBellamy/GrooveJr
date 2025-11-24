//
// Created by ns on 10/16/25.
//

#ifndef MUSICLIBRARYQUERYMODEL_H
#define MUSICLIBRARYQUERYMODEL_H

#include <vector>
#include <cstring>

#include <QObject>
#include <QtSql/qsqlquerymodel.h>
#include <QStandardItemModel>
#include <QVariant>
#include <QColor>
#include <QString>

#include "../../AppState.h"
#include "../../enums/Result.h"

#include "../QSql/SqlWorkerPool.h"
#include "../QSql/SqlWorkerPoolClient.h"
#include "../QSql/SqlQueryModel.h"
#include "MusicLibraryFilters.h"
#include "Constants.h"

namespace Gj {
namespace Gui {
  class SqlWorkerPool;

  class MusicLibraryQueryModel : public SqlQueryModel {

  protected:
    QString id;
    AppState* gAppState;
    MusicLibraryType type;
    MusicLibraryFilters* filters;

    bool isCurrentFilter(const QModelIndex& item, const size_t idCol) const {
      std::vector<ID> ids = filters->filters.at(type).ids;
      return std::find(
          ids.begin(),
          ids.end(),
          index(item.row(), idCol).data()
          ) != ids.end();
    };

  public:
    MusicLibraryQueryModel(
      QObject* parent,
      AppState* gAppState,
      MusicLibraryFilters* filters,
      const MusicLibraryType type,
      const QString& id,
      SqlWorkerPool* sqlWorkerPool
    )
    : SqlQueryModel(parent, gAppState, id, sqlWorkerPool)
    , id(id)
    , gAppState(gAppState)
    , type(type)
    , filters(filters)
    {}

    virtual bool isCurrentlyPlaying(const QModelIndex& item) const = 0;

    QVariant data(const QModelIndex& item, const int role) const {
      if (role == Qt::BackgroundRole && isCurrentlyPlaying(item))
        return QVariant::fromValue(QColor(CURRENTLY_PLAYING_COLOR));

      return {};
    }
};

} // Gui
} // Gj

#endif //MUSICLIBRARYQUERYMODEL_H
