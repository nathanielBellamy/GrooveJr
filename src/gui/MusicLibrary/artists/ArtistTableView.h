//
// Created by ns on 10/11/25.
//

#ifndef ARTISTTABLEVIEW_H
#define ARTISTTABLEVIEW_H

#include "../MusicLibraryFilters.h"
#include "../MusicLibraryTableView.h"

namespace Gj {
namespace Gui {

class ArtistTableView final : public MusicLibraryTableView {

  public:
    ArtistTableView(QWidget* parent, MusicLibraryFilters* filters)
        : MusicLibraryTableView(parent, filters, ARTIST)
        {};
};

} // Gui
} // Gj

#endif //ARTISTTABLEVIEW_H
