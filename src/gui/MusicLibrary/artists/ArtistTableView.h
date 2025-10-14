//
// Created by ns on 10/11/25.
//

#ifndef ARTISTTABLEVIEW_H
#define ARTISTTABLEVIEW_H

#include <QTableView>

#include "../../../enums/Result.h"
#include "ArtistQueryModel.h"
#include "../../../AppState.h"

namespace Gj {
namespace Gui {

class ArtistTableView final : public QTableView {
  ArtistQueryModel* artistQueryModel;
  Result setStyle() {
    setStyleSheet(
      "font-weight: 500; font-size: 12px;"
    );
    return OK;
  }

  public:
    ArtistTableView(QWidget* parent = nullptr)
        : QTableView(parent)
        , artistQueryModel(new ArtistQueryModel(this)) {
      setModel(artistQueryModel);
      setStyle();
    };

    ~ArtistTableView() {
      delete artistQueryModel;
    }

    Result hydrateState(const AppStatePacket& appStatePacket) {
      artistQueryModel->hydrateState(appStatePacket);
      return OK;
    };
};

} // Gui
} // Gj

#endif //ARTISTTABLEVIEW_H
