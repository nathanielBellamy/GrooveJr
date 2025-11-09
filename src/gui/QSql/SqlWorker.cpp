//
// Created by ns on 11/4/25.
//

#include "SqlWorker.h"
#include "SqlWorkerPool.h"

namespace Gj {
namespace Gui {

SqlWorker::~SqlWorker() {
  db.close();
}

Result SqlWorker::connectActions(const SqlWorkerPool* pool) {
  const auto initConnection =
    connect(pool, &SqlWorkerPool::initSqlWorker, [&](const int idxToInit) {
      std::cout << " sql worker " << idx << "init sql worker " << idxToInit << std::endl;
      if (idx == idxToInit) {
        init();
      }
    });

  const auto runQueryConnection =
    connect(pool, &SqlWorkerPool::runQueryWithWorker, [&](const int idxToRun, const QString callerId, const QString& query) {
      if (idx == idxToRun)
        runQuery(callerId, query);
    });

  return OK;
}

void SqlWorker::init() {
  db = QSqlDatabase::addDatabase("QSQLITE", "gj_gui_musiclibrary_worker_" + QString::number(idx));
  db.setDatabaseName("/Users/ns/groovejr.db");
  if (!db.open()) {
    Logging::write(
        Info,
        "Gui::SqlWorker::init",
        "Connection not established. Error: " + db.lastError().text().toStdString()
    );
    emit errorOccurred(db.lastError().text());
    return;
  }
  Logging::write(
      Info,
      "Gui::SqlWorker::init",
      "Connection established"
  );
}

void SqlWorker::runQuery(const QString& callerId, const QString& queryString) {
  std::cout << " SqlWorker" << idx << "runQuery " << callerId.toStdString() << " " << queryString.toStdString() << std::endl;
  busy = true;
  if (!db.open()) {
    Logging::write(
        Error,
        "Gui::SqlWorker::runQuery",
        "Db Connection Not Open"
    );
    busy = false;
    return;
  }

  QSqlQuery query(queryString, db);

  QList<QVariantList> rows;
  while (query.next()) {
    QVariantList row;
    for (int i = 0; i < query.record().count(); ++i)
      row << query.value(i);
    rows << row;
  }
  emit queryResultsReady(callerId, rows);
  busy = false;
  Logging::write(
      Info,
      "Gui::SqlWorker::runQuery",
      "Executed Query. Result Row Count: " + std::to_string(rows.size())
  );
}

} // Gui
} // Gj