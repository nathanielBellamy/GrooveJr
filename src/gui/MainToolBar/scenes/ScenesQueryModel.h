//
// Created by ns on 11/16/25.
//

#ifndef SCENESQUERYMODEL_H
#define SCENESQUERYMODEL_H

#include "../../QSql/SqlQueryModel.h"

namespace Gj {
namespace Gui {

constexpr size_t SCENES_COL_NAME = 0;
constexpr size_t SCENES_COL_ID = 1;

constexpr auto CURRENT_SCENE_COLOR = Qt::cyan;

class ScenesQueryModel final : public SqlQueryModel {

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
  Result refresh(bool hard = false) override;
  Result setHeaders() override;

  bool isCurrentScene(const QModelIndex& item) const;
};

} // Gui
} // Gj

#endif //SCENESQUERYMODEL_H
