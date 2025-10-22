//
// Created by ns on 10/11/25.
//

#ifndef GENRETABLEVIEW_H
#define GENRETABLEVIEW_H

#include "caf/actor_system.hpp"

#include "../MusicLibraryFilters.h"
#include "../MusicLibraryTableView.h"
#include "../MusicLibraryType.h"
#include "GenreQueryModel.h"

namespace Gj {
namespace Gui {

class GenreTableView final : public MusicLibraryTableView {
  public:
    GenreTableView(QWidget* parent, actor_system& actorSystem, Db::Dao* dao, MusicLibraryFilters* filters)
        : MusicLibraryTableView(parent, actorSystem, dao, filters, new GenreQueryModel(parent, filters))
        {};
};

} // Gui
} // Gj

#endif //GENRETABLEVIEW_H
