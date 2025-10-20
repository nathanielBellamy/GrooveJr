//
// Created by ns on 10/11/25.
//

#ifndef QUEUETABLEVIEW_H
#define QUEUETABLEVIEW_H

#include "../MusicLibraryType.h"
#include "../MusicLibraryFilters.h"
#include "../MusicLibraryTableView.h"
#include "QueueQueryModel.h"

namespace Gj {
namespace Gui {

class QueueTableView final : public MusicLibraryTableView {

  public:
    QueueTableView(QWidget* parent, Db::Dao* dao, MusicLibraryFilters* filters)
        : MusicLibraryTableView(parent, dao, QUEUE, AUDIO_FILE_COL_ID, filters, new QueueQueryModel(parent, filters))
        {};
};

} // Gui
} // Gj

#endif //QUEUETABLEVIEW_H
