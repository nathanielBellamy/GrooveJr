//
// Created by ns on 10/11/25.
//

#ifndef GENRETABLEVIEW_H
#define GENRETABLEVIEW_H

#include "../MusicLibraryFilters.h"
#include "../MusicLibraryTableView.h"

namespace Gj {
namespace Gui {

class GenreTableView final : public MusicLibraryTableView {
  public:
    GenreTableView(QWidget* parent, MusicLibraryFilters* filters)
        : MusicLibraryTableView(parent, filters, GENRE)
        {};
};

} // Gui
} // Gj

#endif //GENRETABLEVIEW_H
