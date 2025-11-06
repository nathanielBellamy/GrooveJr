//
// Created by ns on 11/4/25.
//

#include "SqlWorker.h"


namespace Gj {
namespace Gui {

SqlWorker::~SqlWorker() {
  db.close();
}

Result SqlWorker::connectActions(MusicLibraryQueryModel* model) {
  const auto initConnection =
    connect(model, &MusicLibraryQueryModel::initSqlWorker, [&](const QString& callerId) {
        init(callerId);
    });

  const auto runQueryConnection =
    connect(model, &MusicLibraryQueryModel::runQuery, [&](const QString& callerId, const QString& query) {
        runQuery(query);
    });

  return OK;
}

void SqlWorker::init(const QString& newId) {
  id = newId;

  db = QSqlDatabase::addDatabase("QSQLITE", id);
  db.setDatabaseName("/Users/ns/groovejr.db1");
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

void SqlWorker::runQuery(const QString& queryString) {
  if (!db.open()) {
    Logging::write(
        Error,
        "Gui::SqlWorker::runQuery",
        "Db Connection Not Open"
    );
    return;
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
  emit queryResultsReady(rows);
  Logging::write(
      Info,
      "Gui::SqlWorker::runQuery",
      "Executed Query. Result Row Count: " + std::to_string(rows.size())
  );
}

} // Gui
} // Gj