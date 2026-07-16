//
// Created by ns on 11/17/25.
//

#include "SqlQueryModel.h"

namespace Gj {
namespace Gui {
SqlQueryModel::SqlQueryModel(
  QObject* parent,
  QTableView* tableView,
  State::Core* stateCore,
  const QString& id,
  SqlWorkerPool* sqlWorkerPool
)
: SqlWorkerPoolClient(parent)
  , id(id)
  , sqlWorkerPool(sqlWorkerPool)
  , tableView(tableView)
  , stateCore(stateCore) {
  connectToPool();
}

Result SqlQueryModel::connectToPool() {
  const auto queryResultsReadyConnection =
      connect(sqlWorkerPool, &SqlWorkerPool::queryResultsReady,
              [&](const QString& callerId, const QList<QVariantList>& rows) {
                if (callerId != id)
                  return;


                const QPersistentModelIndex persistentTopIndex = tableView->indexAt(QPoint(0, 0));
                clear();
                for (const auto& row: rows) {
                  QList<QStandardItem*> items;
                  for (const auto& val: row)
                    items << new QStandardItem(val.toString());
                  appendRow(items);
                }
                if (persistentTopIndex.isValid())
                  tableView->scrollTo(persistentTopIndex, QAbstractItemView::PositionAtTop);

                setHeaders();
              });
  const auto errorOccurredConnection =
      connect(sqlWorkerPool, &SqlWorkerPool::errorOccurred, this, [&](const QString& error) {
        Logging::write(
          Error,
          "Gui::SqlQueryModel::errorOccurred()",
          "Error: " + error.toStdString()
        );
      });
  return OK;
};
} // Gj
} // Gui
