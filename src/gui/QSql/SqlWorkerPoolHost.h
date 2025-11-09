//
// Created by ns on 11/7/25.
//

#ifndef SQLWORKERPOOLHOST_H
#define SQLWORKERPOOLHOST_H

#include <QObject>
#include <QWidget>

namespace Gj {
namespace Gui {

class SqlWorkerPoolHost : public QWidget {
  Q_OBJECT

public:
  explicit SqlWorkerPoolHost(QWidget* parent = nullptr);

  signals:
    void initSqlWorkerPool();
};

} // Gui
} // Gj

#endif //SQLWORKERPOOLHOST_H
