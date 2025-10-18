//
// Created by ns on 10/13/25.
//

#ifndef PLAYLISTTABLEVIEW_H
#define PLAYLISTTABLEVIEW_H

#include "../MusicLibraryFilters.h"
#include "../MusicLibraryTableView.h"

namespace Gj {
namespace Gui {

class PlaylistTableView final : public MusicLibraryTableView {

public:
    PlaylistTableView(QWidget* parent, MusicLibraryFilters* filters)
        : MusicLibraryTableView(parent, filters, PLAYLIST)
        {};
};

} // Gui
} // Gj

#endif //PLAYLISTTABLEVIEW_H
