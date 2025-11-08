//
// Created by ns on 11/7/25.
//

#ifndef SQLWORKERPOOLCLIENT_H
#define SQLWORKERPOOLCLIENT_H

#include <QObject>
#include <QStandardItemModel>
#include <QString>

namespace Gj {
namespace Gui {

class SqlWorkerPoolClient : public QStandardItemModel {

  Q_OBJECT

public:
  explicit SqlWorkerPoolClient(QObject* parent = nullptr);

  signals:
    void runQuery(const QString& id, const QString& query);

};

} // Gui
} // Gj

#endif //SQLWORKERPOOLCLIENT_H
