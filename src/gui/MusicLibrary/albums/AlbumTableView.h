//
// Created by ns on 10/11/25.
//

#ifndef ALBUMTABLEVIEW_H
#define ALBUMTABLEVIEW_H

#include "caf/actor_system.hpp"

#include <QTimer>

#include "../../../state/Core.h"
#include "../MusicLibraryFilters.h"
#include "../MusicLibraryTableView.h"
#include "../MusicLibraryType.h"
#include "AlbumQueryModel.h"

namespace Gj {
namespace Gui {
using namespace caf;

class AlbumTableView final : public MusicLibraryTableView {
public:
  AlbumTableView(
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
    new AlbumQueryModel(parent, stateCore, filters, sqlWorkerPool),
    filters
  ) {
    refresh(true);
    connect(model, &QAbstractItemModel::modelReset, this, [this]() {
      QTimer::singleShot(0, this, [this]() {
        setColumnHidden(ALBUM_COL_ID, true);
      });
    });
  };
};
} // Gui
} // Gj

#endif //ALBUMTABLEVIEW_H
