//
// Created by ns on 11/16/25.
//

#ifndef SCENESQUERYMODEL_H
#define SCENESQUERYMODEL_H

#include "../../QSql/SqlQueryModel.h"

namespace Gj {
namespace Gui {

class ScenesQueryModel : public SqlQueryModel {

public:
  ScenesQueryModel(
    QObject* parent,
    AppState* gAppState,
    const QString& id,
    SqlWorkerPool* sqlWorkerPool
  )
  : SqlQueryModel(parent, gAppState, id, sqlWorkerPool)
  {
    refresh();
  };

  QVariant data(const QModelIndex& item, int role) const override;
  Result hydrateState(const AppStatePacket& appStatePacket) override;
  Result refresh() override;
  Result setHeaders() override;
};

} // Gui
} // Gj

#endif //SCENESQUERYMODEL_H
