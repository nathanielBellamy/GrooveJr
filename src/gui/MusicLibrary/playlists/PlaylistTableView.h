//
// Created by ns on 10/13/25.
//

#ifndef PLAYLISTTABLEVIEW_H
#define PLAYLISTTABLEVIEW_H

#include "caf/actor_system.hpp"

#include "../../../AppState.h"
#include "../MusicLibraryFilters.h"
#include "../MusicLibraryTableView.h"
#include "../MusicLibraryType.h"
#include "PlaylistQueryModel.h"

namespace Gj {
namespace Gui {

class PlaylistTableView final : public MusicLibraryTableView {

public:
    PlaylistTableView(QWidget* parent, actor_system& actorSystem, Db::Dao* dao, AppState* gAppState, MusicLibraryFilters* filters)
        : MusicLibraryTableView(parent, actorSystem, dao, gAppState, filters, new PlaylistQueryModel(parent, gAppState, filters))
        {};
};

} // Gui
} // Gj

#endif //PLAYLISTTABLEVIEW_H
