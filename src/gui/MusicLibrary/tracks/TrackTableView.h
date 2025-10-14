//
// Created by ns on 10/11/25.
//

#ifndef TRACKTABLEVIEW_H
#define TRACKTABLEVIEW_H

#include <QTableView>

#include "../../../enums/Result.h"
#include "../../../AppState.h"

#include "../MusicLibraryFilters.h"
#include "TrackQueryModel.h"

namespace Gj {
namespace Gui {

class TrackTableView final : public QTableView {
  TrackQueryModel* trackQueryModel;
  Result setStyle() {
    setStyleSheet(
      "font-weight: 500; font-size: 12px;"
    );
    return OK;
  }

  public:
    TrackTableView(QWidget* parent, MusicLibraryFilters* filters)
        : QTableView(parent)
        , trackQueryModel(new TrackQueryModel(this, filters)) {
      setModel(trackQueryModel);
      setStyle();
    };

    ~TrackTableView() {
      delete trackQueryModel;
    }

    Result refresh() {
      trackQueryModel->refresh();
      return OK;
    }

    Result hydrateState(const AppStatePacket& appStatePacket) {
      trackQueryModel->hydrateState(appStatePacket);
      return OK;
    };
};

} // Gui
} // Gj

#endif //TRACKTABLEVIEW_H
