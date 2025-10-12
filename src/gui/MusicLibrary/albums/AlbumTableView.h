//
// Created by ns on 10/11/25.
//

#ifndef ALBUMTABLEVIEW_H
#define ALBUMTABLEVIEW_H

#include <QTableView>

#include "../../../enums/Result.h"
#include "AlbumQueryModel.h"
#include "../../../AppState.h"

namespace Gj {
namespace Gui {

class AlbumTableView final : public QTableView {
  AlbumQueryModel* albumQueryModel;

  public:
    AlbumTableView(QWidget* parent = nullptr)
        : QTableView(parent)
        , albumQueryModel(new AlbumQueryModel(this)) {
      setModel(albumQueryModel);
    };

    ~AlbumTableView() {
      delete albumQueryModel;
    }

    Result hydrateState(const AppStatePacket& appStatePacket) {
      albumQueryModel->hydrateState(appStatePacket);
      return OK;
    };
};

} // Gui
} // Gj

#endif //ALBUMTABLEVIEW_H
