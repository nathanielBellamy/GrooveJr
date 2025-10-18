//
// Created by ns on 10/11/25.
//

#ifndef ALBUMTABLEVIEW_H
#define ALBUMTABLEVIEW_H

#include "../MusicLibraryFilters.h"
#include "../MusicLibraryTableView.h"
#include "AlbumQueryModel.h"

namespace Gj {
namespace Gui {

class AlbumTableView final : public MusicLibraryTableView {

  public:
    AlbumTableView(QWidget* parent, MusicLibraryFilters* filters)
        : MusicLibraryTableView(parent, filters, ALBUM)
        {};
};

} // Gui
} // Gj

#endif //ALBUMTABLEVIEW_H
