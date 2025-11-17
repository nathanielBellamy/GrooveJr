//
// Created by ns on 11/17/25.
//

#include "SqlQueryModel.h"

namespace Gj {
namespace Gui {

SqlQueryModel::SqlQueryModel(
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

} // Gj
} // Gui