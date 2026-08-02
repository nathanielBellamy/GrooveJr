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

                const QModelIndex topIndex = tableView->indexAt(QPoint(0, 0));
                const int savedRow = topIndex.isValid() ? topIndex.row() : 0;
                clear();
                for (const auto& row: rows) {
                  QList<QStandardItem*> items;
                  for (const auto& val: row)
                    items << new QStandardItem(val.toString());
                  appendRow(items);
                }
                setHeaders();
                if (savedRow > 0 && savedRow < rowCount()) {
                  const QModelIndex restoreIndex = index(savedRow, 0);
                  tableView->scrollTo(restoreIndex, QAbstractItemView::PositionAtTop);
                }
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
