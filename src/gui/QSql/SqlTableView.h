//
// Created by ns on 11/15/25.
//

#ifndef GJSQLTABLEVIEW_H
#define GJSQLTABLEVIEW_H

#include <QMenu>
#include <QMouseEvent>
#include <QTableView>

#include "caf/actor_system.hpp"

#include "../../enums/Result.h"
#include "../../state/Core.h"
#include "../../db/Dao.h"

#include "SqlQueryModel.h"

namespace Gj {
namespace Gui {
using namespace caf;

class SqlTableView : public QTableView {
  Result setStyle() {
    setStyleSheet(
      "font-weight: 500; font-size: 12px;"
    );
    return OK;
  }

protected:
  actor_system& actorSystem;
  Db::Dao* dao;
  Gj::State::Core* stateCore;
  SqlQueryModel* model;

public:
  SqlTableView(
    QWidget* parent,
    actor_system& actorSystem,
    Db::Dao* dao,
    Gj::State::Core* stateCore,
    SqlQueryModel* model
  );

  ~SqlTableView() override {
    delete model;
    QTableView::~QTableView();
  }

  SqlQueryModel* getModel() const {
    return model;
  }

  Result hydrateState(const Gj::State::Packet& statePacket) const {
    model->hydrateState(statePacket);
    refresh();
    return OK;
  }

  Result refresh(const bool hard = false) const {
    model->refresh(hard);
    return OK;
  }
};
} // Gui
} // Gj

#endif //GJSQLTABLEVIEW_H
