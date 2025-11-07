//
// Created by ns on 11/4/25.
//

#ifndef GJGUISQLWORKERPOOL_H
#define GJGUISQLWORKERPOOL_H

#include <queue>

#include <QObject>
#include <QThread>

#include "../../enums/Result.h"
#include "SqlWorker.h"

namespace Gj {
namespace Gui {

struct QueryQueueItem {
  QString callerId;
  QString query;
};

constexpr size_t WORKER_POOL_SIZE = 3;

class SqlWorkerPool final : public QObject {
  Q_OBJECT

  std::queue<QueryQueueItem> queryQueue;
  QThread threads[WORKER_POOL_SIZE]{};
  SqlWorker* workers[WORKER_POOL_SIZE]{ nullptr };
  Result addQueryToQueue;
  Result connectActions();

public:
  explicit SqlWorkerPool()
    : QObject(nullptr) {

    for (int i = 0; i < WORKER_POOL_SIZE; i++) {
      workers[i] = new SqlWorker(this, i);
      workers[i]->moveToThread(&threads[0]);
      emit initSqlWorker(i);
    }
  };
  ~SqlWorkerPool() {
    for (auto thread : threads)
      thread.quit();
  }

  void run() {
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
    }
  };

signals:
  // down signals
  void initSqlWorker(int idxToInit);
  void runQueryWithWorker(int workerIdx, const QString& callerId, const QString& query);

  // up signals
  void queryResultsReady(const QString& callerId, const QList<QVariantList>& rows);
  void errorOccurred(const QString& callerId, const QString& error);

public slots:
  void runQuery(const QString& callerId, const QString& query) {
    queryQueue.push(QueryQueueItem{callerId, query});
  }
};

};

}  // Gui
} // Gj


#endif //GJGUISQLWORKERPOOL_H
