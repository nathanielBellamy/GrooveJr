//
// Created by ns on 10/13/25.
//

#ifndef PLAYLISTTABLEVIEW_H
#define PLAYLISTTABLEVIEW_H

#include "caf/actor_system.hpp"

#include "../../../state/Core.h"
#include "../MusicLibraryFilters.h"
#include "../MusicLibraryTableView.h"
#include "../MusicLibraryType.h"
#include "PlaylistQueryModel.h"

namespace Gj {
namespace Gui {
class PlaylistTableView final : public MusicLibraryTableView {
public:
  PlaylistTableView(
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
    new PlaylistQueryModel(parent, stateCore, filters, sqlWorkerPool),
    filters
  ) {
    refresh(true);
  };
};
} // Gui
} // Gj

#endif //PLAYLISTTABLEVIEW_H
