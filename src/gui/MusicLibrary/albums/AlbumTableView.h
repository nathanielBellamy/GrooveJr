//
// Created by ns on 10/11/25.
//

#ifndef ALBUMTABLEVIEW_H
#define ALBUMTABLEVIEW_H

#include "caf/actor_system.hpp"

#include "../../../state/AppState.h"
#include "../MusicLibraryFilters.h"
#include "../MusicLibraryTableView.h"
#include "../MusicLibraryType.h"
#include "AlbumQueryModel.h"

namespace Gj {
namespace Gui {
using namespace caf;

class AlbumTableView final : public MusicLibraryTableView {
public:
  AlbumTableView(
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
    new AlbumQueryModel(parent, gAppState, filters, sqlWorkerPool),
    filters
  ) {
  };
};
} // Gui
} // Gj

#endif //ALBUMTABLEVIEW_H
