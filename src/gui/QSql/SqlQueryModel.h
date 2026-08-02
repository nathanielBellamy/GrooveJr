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
#include "QTableView"

namespace Gj {
namespace Gui {
class SqlWorkerPool;

class SqlQueryModel : public SqlWorkerPoolClient {
  std::string previousQuery = "";

  Result connectToPool();

protected:
  QString id;
  SqlWorkerPool* sqlWorkerPool;
  QTableView* tableView;
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
    QTableView* tableView,
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
