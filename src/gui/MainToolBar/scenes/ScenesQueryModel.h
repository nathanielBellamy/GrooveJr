//
// Created by ns on 11/16/25.
//

#ifndef SCENESQUERYMODEL_H
#define SCENESQUERYMODEL_H

#include "../../QSql/SqlQueryModel.h"

namespace Gj {
namespace Gui {
constexpr size_t SCENES_COL_SCENE_ID = 0;
constexpr size_t SCENES_COL_NAME = 1;
constexpr size_t SCENES_COL_VERSION = 2;
constexpr size_t SCENES_COL_ID = 3;

constexpr auto CURRENT_SCENE_COLOR = Qt::darkCyan;

class ScenesQueryModel final : public SqlQueryModel {
public:
  ScenesQueryModel(
    QObject* parent,
    State::Core* stateCore,
    const QString& id,
    SqlWorkerPool* sqlWorkerPool
  )
  : SqlQueryModel(parent, stateCore, id, sqlWorkerPool) {
    refresh();
  };

  QVariant data(const QModelIndex& item, int role) const override;

  Result hydrateState(const State::Packet& statePacket) override;

  Result refresh(bool hard = false) override;

  Result setHeaders() override;

  bool isCurrentScene(const QModelIndex& item) const;
};
} // Gui
} // Gj

#endif //SCENESQUERYMODEL_H
