//
// Created by ns on 10/11/25.
//

#ifndef CACHETABLEVIEW_H
#define CACHETABLEVIEW_H

#include "../../../state/AppState.h"
#include "../MusicLibraryType.h"
#include "../MusicLibraryFilters.h"
#include "../MusicLibraryTableView.h"
#include "CacheQueryModel.h"

namespace Gj {
namespace Gui {
class CacheTableView final : public MusicLibraryTableView {
public:
  CacheTableView(
    QWidget* parent,
    actor_system& actorSystem,
    Db::Dao* dao,
    AppState* gAppState,
    MusicLibraryFilters* filters,
    SqlWorkerPool* sqlWorkerPool
  )
  : MusicLibraryTableView(
    parent,
    actorSystem,
    dao,
    gAppState,
    new CacheQueryModel(parent, gAppState, filters, sqlWorkerPool),
    filters
  ) {
  };
};
} // Gui
} // Gj

#endif //CACHETABLEVIEW_H
