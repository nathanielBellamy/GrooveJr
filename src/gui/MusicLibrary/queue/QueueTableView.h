//
// Created by ns on 10/11/25.
//

#ifndef QUEUETABLEVIEW_H
#define QUEUETABLEVIEW_H

#include "../../../AppState.h"
#include "../MusicLibraryType.h"
#include "../MusicLibraryFilters.h"
#include "../MusicLibraryTableView.h"
#include "QueueQueryModel.h"

namespace Gj {
namespace Gui {

class QueueTableView final : public MusicLibraryTableView {

  public:
    QueueTableView(QWidget* parent, actor_system& actorSystem, Db::Dao* dao, AppState* gAppState, MusicLibraryFilters* filters)
        : MusicLibraryTableView(parent, actorSystem, dao, gAppState, filters, new QueueQueryModel(parent, gAppState, filters))
        {};
};

} // Gui
} // Gj

#endif //QUEUETABLEVIEW_H
