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
#include "../MusicLibrary/MusicLibraryQueryModel.h"

namespace Gj {
namespace Gui {

class SqlWorker : public QObject {
  Q_OBJECT

  QString id;

public:
  explicit SqlWorker(MusicLibraryQueryModel* model)
      : QObject(nullptr)
      {
    connectActions(model);
  };
  ~SqlWorker();

  Result connectActions(MusicLibraryQueryModel* model);

  public slots:
    void init(const QString& newId);
    void runQuery(const QString& queryString);

  signals:
    void queryResultsReady(const QList<QVariantList>& rows);
    void errorOccurred(const QString& error);

private:
  QSqlDatabase db;
};

} // Gui
} // Gj

#endif //GJGUISQLWORKER_H
