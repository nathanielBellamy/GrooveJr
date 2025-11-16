//
// Created by ns on 10/17/25.
//

#ifndef MUSICLIBRARYTABLEVIEW_H
#define MUSICLIBRARYTABLEVIEW_H

#include <QMenu>
#include <QMouseEvent>

#include "caf/actor_system.hpp"

#include "../../enums/Result.h"
#include "../../AppState.h"
#include "../../db/Dao.h"

#include "../QSql/SqlTableView.h"
#include "MusicLibraryQueryModel.h"
#include "MusicLibraryFilters.h"


namespace Gj {
namespace Gui {

using namespace caf;

class MusicLibraryTableView : public SqlTableView {
  protected:
    QMenu* menu;
    MusicLibraryFilters* filters;

  public:
    MusicLibraryTableView(
      QWidget* parent,
      actor_system& actorSystem,
      Db::Dao* dao,
      AppState* gAppState,
      SqlQueryModel* model,
      MusicLibraryFilters* filters
    )
    : SqlTableView(parent, actorSystem, dao, gAppState, model)
    , menu(nullptr)
    , filters(filters)
    {};
};

} // Gui
} // Gj

#endif //MUSICLIBRARYTABLEVIEW_H
