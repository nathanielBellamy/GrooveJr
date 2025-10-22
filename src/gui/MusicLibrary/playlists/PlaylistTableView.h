//
// Created by ns on 10/13/25.
//

#ifndef PLAYLISTTABLEVIEW_H
#define PLAYLISTTABLEVIEW_H

#include "caf/actor_system.hpp"

#include "../MusicLibraryFilters.h"
#include "../MusicLibraryTableView.h"
#include "../MusicLibraryType.h"
#include "PlaylistQueryModel.h"

namespace Gj {
namespace Gui {

class PlaylistTableView final : public MusicLibraryTableView {

public:
    PlaylistTableView(QWidget* parent, actor_system& actorSystem, Db::Dao* dao, MusicLibraryFilters* filters)
        : MusicLibraryTableView(parent, actorSystem, dao, filters, new PlaylistQueryModel(parent, filters))
        {};
};

} // Gui
} // Gj

#endif //PLAYLISTTABLEVIEW_H
