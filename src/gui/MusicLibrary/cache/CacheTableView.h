//
// Created by ns on 10/11/25.
//

#ifndef CACHETABLEVIEW_H
#define CACHETABLEVIEW_H

#include "../../../state/Core.h"
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
    Gj::State::Core* stateCore,
    MusicLibraryFilters* filters,
    SqlWorkerPool* sqlWorkerPool
  )
  : MusicLibraryTableView(
    parent,
    actorSystem,
    dao,
    stateCore,
    new CacheQueryModel(parent, stateCore, filters, sqlWorkerPool),
    filters
  ) {
  };
};
} // Gui
} // Gj

#endif //CACHETABLEVIEW_H
