//
// Created by ns on 10/11/25.
//

#ifndef ALBUMTABLEVIEW_H
#define ALBUMTABLEVIEW_H

#include <QTableView>

#include "../../../enums/Result.h"
#include "AlbumQueryModel.h"
#include "../../../AppState.h"

#include "../MusicLibraryFilters.h"

namespace Gj {
namespace Gui {

class AlbumTableView final : public QTableView {
  AlbumQueryModel* albumQueryModel;
  Result setStyle() {
    setStyleSheet(
      "font-weight: 500; font-size: 12px;"
    );
    return OK;
  }

  public:
    AlbumTableView(QWidget* parent, MusicLibraryFilters* filters)
        : QTableView(parent)
        , albumQueryModel(new AlbumQueryModel(this, filters)) {
      setModel(albumQueryModel);
      setStyle();
    };

    ~AlbumTableView() {
      delete albumQueryModel;
    }

    Result hydrateState(const AppStatePacket& appStatePacket) {
      albumQueryModel->hydrateState(appStatePacket);
      return OK;
    };

    Result refresh() {
      albumQueryModel->refresh();
      return OK;
    }
};

} // Gui
} // Gj

#endif //ALBUMTABLEVIEW_H
