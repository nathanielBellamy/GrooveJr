//
// Created by ns on 11/4/25.
//

#ifndef GJGUISQLWORKER_H
#define GJGUISQLWORKER_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QString>

#include "../../Logging.h"
#include "../../enums/Result.h"

namespace Gj {
namespace Gui {

class SqlWorkerPool; // forward decl

class SqlWorker : public QObject {
  Q_OBJECT
  int idx;
  std::atomic<bool> busy;
  Result connectActions(const SqlWorkerPool* pool);

public:
  explicit SqlWorker(SqlWorkerPool* pool, const int idx)
      : QObject(nullptr)
      , idx(idx)
      {
    connectActions(pool);
  };
  ~SqlWorker();

  int getIdx() const {
    return idx;
  };

  bool isBusy() const {
    return busy.load();
  };

public slots:
  void init();
  void runQuery(const QString& callerId, const QString& queryString);

signals:
  void queryResultsReady(const QString& callerId, const QList<QVariantList>& rows);
  void errorOccurred(const QString& error);

private:
  QSqlDatabase db;
};

} // Gui
} // Gj

#endif //GJGUISQLWORKER_H
