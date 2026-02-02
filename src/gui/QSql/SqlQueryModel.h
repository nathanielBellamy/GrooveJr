//
// Created by ns on 11/15/25.
//

#ifndef GJSQLQUERYMODEL_H
#define GJSQLQUERYMODEL_H

#include <cstring>

#include "../../state/Core.h"
#include "../../enums/Result.h"

#include "SqlWorkerPool.h"
#include "SqlWorkerPoolClient.h"

namespace Gj {
namespace Gui {
class SqlWorkerPool;

class SqlQueryModel : public SqlWorkerPoolClient {
  std::string previousQuery = "";

  Result connectToPool() {
    const auto queryResultsReadyConnection =
        connect(sqlWorkerPool, &SqlWorkerPool::queryResultsReady,
                [&](const QString& callerId, const QList<QVariantList>& rows) {
                  if (callerId != id)
                    return;

                  clear();
                  for (const auto& row: rows) {
                    QList<QStandardItem*> items;
                    for (const auto& val: row)
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
  State::Core* stateCore;

  Result setPreviousQuery(std::string newQueryString) {
    previousQuery = newQueryString;
    return OK;
  };

  bool queryHasChanged(std::string newQuery) {
    // todo
    return true;

    if (previousQuery.compare(newQuery) == 0) {
      setPreviousQuery(newQuery);
      return true;
    }

    return false;
  }

public:
  SqlQueryModel(
    QObject* parent,
    State::Core* stateCore,
    const QString& id,
    SqlWorkerPool* sqlWorkerPool
  );

  virtual Result hydrateState(const State::Packet& statePacket) = 0;

  virtual Result refresh(bool hard = false) = 0;

  virtual Result setHeaders() = 0;
};
} // Gui
} // Gj

#endif //GJSQLQUERYMODEL_H
