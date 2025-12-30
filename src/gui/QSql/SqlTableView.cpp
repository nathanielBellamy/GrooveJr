//
// Created by ns on 11/17/25.
//

#include "SqlTableView.h"

namespace Gj {
namespace Gui {
SqlTableView::SqlTableView(
  QWidget* parent,
  actor_system& actorSystem,
  Db::Dao* dao,
  Gj::State::Core* stateCore,
  SqlQueryModel* model
)
: QTableView(parent)
  , actorSystem(actorSystem)
  , dao(dao)
  , stateCore(stateCore)
  , model(model) {
  setContextMenuPolicy(Qt::CustomContextMenu);
  setModel(this->model);
  setStyle();
};
} // Gui
} // Gj
