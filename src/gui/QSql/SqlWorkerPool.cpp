//
// Created by ns on 11/4/25.
//

#include "SqlWorkerPool.h"

namespace Gj {
namespace Gui {

Result SqlWorkerPool::connectActions() {
  for (const auto worker : workers) {
    connect(worker, &SqlWorker::queryResultsReady, [&](const QString& callerId, const QList<QVariantList>& rows) {
      emit queryResultsReady(callerId, rows);
    });
  }
}

}
}