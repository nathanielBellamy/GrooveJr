//
// Created by ns on 11/15/25.
//

#ifndef GJSQLQUERYMODEL_H
#define GJSQLQUERYMODEL_H

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

#include "SqlWorkerPool.h"
#include "SqlWorkerPoolClient.h"

namespace Gj {
namespace Gui {
  class SqlWorkerPool;

  class SqlQueryModel : public SqlWorkerPoolClient {
    const char* previousQuery = "";
    Result connectToPool() {
      const auto queryResultsReadyConnection =
        connect(sqlWorkerPool, &SqlWorkerPool::queryResultsReady, [&](const QString& callerId, const QList<QVariantList>& rows) {
          if (callerId != id)
            return;

          clear();
          for (const auto& row : rows) {
            QList<QStandardItem*> items;
            for (const auto& val : row)
              items << new QStandardItem(val.toString());
            appendRow(items);
          }

          setHeaders();
        });

       const auto errorOccurredConnection =
         connect(sqlWorkerPool, &SqlWorkerPool::errorOccurred, this, [&](const QString& error) {
           Logging::write(
             Error,
             "Gui::SqlQueryModel::errorOccurred()",
             "Error: " + error.toStdString()
           );
         });
      return OK;
    };

  protected:
    QString id;
    SqlWorkerPool* sqlWorkerPool;
    AppState* gAppState;

    Result setPreviousQuery(const std::string& newQueryString) {
      previousQuery = newQueryString.c_str();
      return OK;
    };

    bool queryHasChanged(const char* newQuery) {
      if (previousQuery == nullptr) {
        setPreviousQuery(std::string(newQuery));
        return true;
      }

      return std::strcmp(previousQuery, newQuery) != 0;
    }

  public:
    SqlQueryModel(
      QObject* parent,
      AppState* gAppState,
      const QString& id,
      SqlWorkerPool* sqlWorkerPool
    )
    : SqlWorkerPoolClient(parent)
    , id(id)
    , sqlWorkerPool(sqlWorkerPool)
    , gAppState(gAppState)
    {
      connectToPool();
    }

    virtual Result hydrateState(const AppStatePacket& appStatePacket) = 0;
    virtual Result refresh() = 0;
    virtual Result setHeaders() = 0;
};

} // Gui
} // Gj

#endif //GJSQLQUERYMODEL_H
