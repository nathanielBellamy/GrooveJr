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
      if (idx == idxToInit)
        init();
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
  busy = true;
  if (!db.open()) {
    Logging::write(
        Error,
        "Gui::SqlWorker::runQuery",
        "Db Connection Not Open"
    );
    return;
    busy = false;
  }

  QSqlQuery query(queryString);
//  if (!query.exec(queryString)) {
//    std::cout << "sqlworkerqueryerror - " << query.lastError().text().toStdString() << std::endl;
//    emit errorOccurred(query.lastError().text());
//    return;
//  }

  QList<QVariantList> rows;
  while (query.next()) {
    std::cout << "query.next(): " << query.value(0).toString().toStdString() << std::endl;
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