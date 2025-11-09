//
// Created by ns on 11/4/25.
//

#ifndef GJGUISQLWORKERPOOL_H
#define GJGUISQLWORKERPOOL_H

#include <queue>
#include <vector>

#include <QObject>
#include <QThread>

#include "../../enums/Result.h"
#include "SqlWorker.h"
#include "SqlWorkerPoolClient.h"
#include "SqlWorkerPoolHost.h"

namespace Gj {
namespace Gui {

struct QueryQueueItem {
  QString callerId;
  QString query;
};

constexpr size_t WORKER_POOL_SIZE = 3;

class SqlWorkerPool final : public QObject {
  Q_OBJECT

  std::vector<QObject*> clients;
  SqlWorkerPoolHost* host;
  std::queue<QueryQueueItem> queryQueue;
  QThread* threads[WORKER_POOL_SIZE];
  SqlWorker* workers[WORKER_POOL_SIZE]{ nullptr };
  Result addQueryToQueue;
  Result connectActions();
  void runPool();

public:
  explicit SqlWorkerPool(SqlWorkerPoolHost* host);
  ~SqlWorkerPool();

  void connectClient(SqlWorkerPoolClient* client) { // todo: generalize client to interface
    clients.push_back(client);
    const auto clientConnection =
      connect(client, &SqlWorkerPoolClient::runQuery, this, &SqlWorkerPool::runQuery);
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
    int workerIdxToUse = -1;
    for (const auto worker : workers) {
      if (!worker->isBusy()) {
        workerIdxToUse = worker->getIdx();
        break;
      }
    }
    emit runQueryWithWorker(workerIdxToUse, callerId, query);
  }
};

}  // Gui
} // Gj


#endif //GJGUISQLWORKERPOOL_H
