//
// Created by ns on 10/11/25.
//

#ifndef ALBUMTABLEVIEW_H
#define ALBUMTABLEVIEW_H

#include "../MusicLibraryFilters.h"
#include "../MusicLibraryTableView.h"
#include "../MusicLibraryType.h"
#include "AlbumQueryModel.h"

namespace Gj {
namespace Gui {

class AlbumTableView final : public MusicLibraryTableView {

  public:
    AlbumTableView(QWidget* parent, Db::Dao* dao, MusicLibraryFilters* filters)
        : MusicLibraryTableView(parent, dao, ALBUM, ALBUM_COL_ID, filters, new AlbumQueryModel(parent, filters))
        {};
};

} // Gui
} // Gj

#endif //ALBUMTABLEVIEW_H
