//
// Created by ns on 11/16/25.
//

#ifndef SCENESTABLEVIEW_H
#define SCENESTABLEVIEW_H

#include "../../QSql/SqlTableView.h"
#include "ScenesQueryModel.h"

namespace Gj {
namespace Gui {
class ScenesTableView final : public SqlTableView {
  QAction* sceneLoadAction;

  Result setStyle() {
    setMaximumHeight(80);
    return OK;
  }

  void mouseDoubleClickEvent(QMouseEvent* event) override {
    if (const QModelIndex clickedIndex = indexAt(event->pos()); clickedIndex.isValid()) {
      const SqlQueryModel* model = getModel();
      const ID sceneDbId = model->index(clickedIndex.row(), SCENES_COL_ID).data().toULongLong();
      sceneLoadAction->setData(sceneDbId);
      sceneLoadAction->trigger();
    }
  }

public:
  ScenesTableView(
    QWidget* parent,
    actor_system& actorSystem,
    Db::Dao* dao,
    Gj::State::Core* stateCore,
    SqlWorkerPool* workerPool,
    QAction* sceneLoadAction
  )
  : SqlTableView(
      parent,
      actorSystem,
      dao,
      stateCore,
      new ScenesQueryModel(parent, stateCore, QString("ScenesQueryModel"), workerPool)
    )
    , sceneLoadAction(sceneLoadAction) {
    workerPool->connectClient(model);
    setStyle();
    refresh(true);
  };
};
} // Gui
} // Gj

#endif //SCENESTABLEVIEW_H
