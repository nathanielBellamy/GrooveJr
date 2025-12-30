//
// Created by ns on 10/11/25.
//

#ifndef ARTISTTABLEVIEW_H
#define ARTISTTABLEVIEW_H

#include "caf/actor_system.hpp"

#include "../MusicLibraryFilters.h"
#include "../MusicLibraryTableView.h"
#include "../MusicLibraryType.h"
#include "ArtistQueryModel.h"

namespace Gj {
namespace Gui {
class ArtistTableView final : public MusicLibraryTableView {
public:
  ArtistTableView(
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
    new ArtistQueryModel(parent, stateCore, filters, sqlWorkerPool),
    filters
  ) {
  };
};
} // Gui
} // Gj

#endif //ARTISTTABLEVIEW_H
