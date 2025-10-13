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

  public:
    GenreTableView(QWidget* parent = nullptr)
        : QTableView(parent)
        , genreQueryModel(new GenreQueryModel(this)) {
      setModel(genreQueryModel);
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
