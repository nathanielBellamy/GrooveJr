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
  Result setStyle() {
    setMaximumHeight(60);
    return OK;
  }

  public:
    ScenesTableView(
      QWidget* parent,
      actor_system& actorSystem,
      Db::Dao* dao,
      AppState* gAppState,
      SqlWorkerPool* workerPool
    )
    : SqlTableView(
          parent,
          actorSystem,
          dao,
          gAppState,
          new ScenesQueryModel(parent, gAppState, QString("ScenesQueryModel"), workerPool)
      ) {

      workerPool->connectClient(model);
      setStyle();
      refresh();
    };
};

} // Gui
} // Gj

#endif //SCENESTABLEVIEW_H
