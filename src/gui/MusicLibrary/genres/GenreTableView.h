//
// Created by ns on 10/11/25.
//

#ifndef GENRETABLEVIEW_H
#define GENRETABLEVIEW_H

#include <QTableView>

#include "../../../enums/Result.h"
#include "GenreQueryModel.h"
#include "../../../AppState.h"

namespace Gj {
namespace Gui {

class GenreTableView final : public QTableView {
  GenreQueryModel* genreQueryModel;
  Result setStyle() {
    setStyleSheet(
      "font-weight: 500; font-size: 12px;"
    );
    return OK;
  }

  public:
    GenreTableView(QWidget* parent = nullptr)
        : QTableView(parent)
        , genreQueryModel(new GenreQueryModel(this)) {
      setModel(genreQueryModel);
      setStyle();
    };

    ~GenreTableView() {
      delete genreQueryModel;
    }

    Result hydrateState(const AppStatePacket& appStatePacket) {
      genreQueryModel->hydrateState(appStatePacket);
      return OK;
    };
};

} // Gui
} // Gj

#endif //GENRETABLEVIEW_H
