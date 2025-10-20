//
// Created by ns on 10/13/25.
//

#ifndef PLAYLISTTABLEVIEW_H
#define PLAYLISTTABLEVIEW_H

#include "../MusicLibraryFilters.h"
#include "../MusicLibraryTableView.h"
#include "../MusicLibraryType.h"
#include "PlaylistQueryModel.h"

namespace Gj {
namespace Gui {

class PlaylistTableView final : public MusicLibraryTableView {

public:
    PlaylistTableView(QWidget* parent, Db::Dao* dao, MusicLibraryFilters* filters)
        : MusicLibraryTableView(parent, dao, PLAYLIST, PLAYLIST_COL_ID, filters, new PlaylistQueryModel(parent, filters))
        {};
};

} // Gui
} // Gj

#endif //PLAYLISTTABLEVIEW_H
