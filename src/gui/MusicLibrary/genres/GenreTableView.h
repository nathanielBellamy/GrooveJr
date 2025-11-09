//
// Created by ns on 10/11/25.
//

#ifndef GENRETABLEVIEW_H
#define GENRETABLEVIEW_H

#include "caf/actor_system.hpp"

#include "../../../AppState.h"
#include "../MusicLibraryFilters.h"
#include "../MusicLibraryTableView.h"
#include "../MusicLibraryType.h"
#include "GenreQueryModel.h"

namespace Gj {
namespace Gui {

class GenreTableView final : public MusicLibraryTableView {
  public:
    GenreTableView(
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
      filters,
      new GenreQueryModel(parent, gAppState, filters, sqlWorkerPool)
    )
    {};
};

} // Gui
} // Gj

#endif //GENRETABLEVIEW_H
