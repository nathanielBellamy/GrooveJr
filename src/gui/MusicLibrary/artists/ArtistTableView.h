//
// Created by ns on 10/11/25.
//

#ifndef ARTISTTABLEVIEW_H
#define ARTISTTABLEVIEW_H

#include "caf/actor_system.hpp"

#include <QTimer>

#include "../MusicLibraryFilters.h"
#include "../MusicLibraryTableView.h"
#include "../MusicLibraryType.h"
#include "ArtistQueryModel.h"

namespace Gj {
namespace Gui {
class ArtistTableView final : public MusicLibraryTableView {
public:
  ArtistTableView(
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
    new ArtistQueryModel(parent, stateCore, filters, sqlWorkerPool),
    filters
  ) {
    refresh(true);
    connect(model, &QAbstractItemModel::modelReset, this, [this]() {
      QTimer::singleShot(0, this, [this]() {
        setColumnHidden(ARTIST_COL_ID, true);
      });
    });
  };
};
} // Gui
} // Gj

#endif //ARTISTTABLEVIEW_H
