//
// Created by ns on 10/11/25.
//

#ifndef TRACKTABLEVIEW_H
#define TRACKTABLEVIEW_H

#include <QTableView>

#include "../../../enums/Result.h"
#include "TrackQueryModel.h"
#include "../../../AppState.h"

namespace Gj {
namespace Gui {

class TrackTableView final : public QTableView {
  TrackQueryModel* trackQueryModel;

  public:
    TrackTableView(QWidget* parent = nullptr)
        : QTableView(parent)
        , trackQueryModel(new TrackQueryModel(this)) {
      setModel(trackQueryModel);
    };

    ~TrackTableView() {
      delete trackQueryModel;
    }

    Result hydrateState(const AppStatePacket& appStatePacket) {
      trackQueryModel->hydrateState(appStatePacket);
      return OK;
    };
};

} // Gui
} // Gj

#endif //TRACKTABLEVIEW_H
