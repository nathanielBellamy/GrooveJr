//
// Created by ns on 11/4/25.
//

#include "SqlWorkerPool.h"

namespace Gj {
namespace Gui {

SqlWorkerPool::SqlWorkerPool(SqlWorkerPoolHost* host)
  : QObject(nullptr) // this will move to another thread
  , host(host)
  {

  for (int i = 0; i < WORKER_POOL_SIZE; i++) {
    threads[i] = new QThread();
    workers[i] = new SqlWorker(this, i);
    workers[i]->moveToThread(threads[0]);
    emit initSqlWorker(i);
  }

  connectActions();

  Logging::write(
    Info,
    "Gui::SqlWorkerPool::SqlWorkerPool()",
    "Instantiated SqlWorkerPool."
  );
};

SqlWorkerPool::~SqlWorkerPool() {
  for (int i = 0; i < WORKER_POOL_SIZE; i++) {
    delete workers[i];
    threads[i]->quit();
    delete threads[i];
  }
}

Result SqlWorkerPool::connectActions() {
  connect(host, &SqlWorkerPoolHost::initSqlWorkerPool, [&]() {
    // runPool();
  });

  for (const auto worker : workers) {
    connect(worker, &SqlWorker::queryResultsReady, [&](const QString& callerId, const QList<QVariantList>& rows) {
      emit queryResultsReady(callerId, rows);
    });
  }

  return OK;
}

void SqlWorkerPool::runPool() {
  while (true) {
    if (queryQueue.empty())
      continue;

    int workerIdxToUse = -1;
    for (const auto worker : workers) {
      if (worker->isBusy())
        continue;
      workerIdxToUse = worker->getIdx();
    }

    if (workerIdxToUse == -1)
      continue;

    auto [callerId, query] = queryQueue.front();
    queryQueue.pop();
    emit runQueryWithWorker(workerIdxToUse, callerId, query);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
};

}
}