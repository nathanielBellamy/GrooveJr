//
// Created by ns on 10/17/25.
//

#ifndef MUSICLIBRARYTABLEVIEW_H
#define MUSICLIBRARYTABLEVIEW_H

#include <QTableView>

#include "../../enums/Result.h"
#include "../../AppState.h"

#include "MusicLibraryQueryModel.h"
#include "MusicLibraryFilters.h"

namespace Gj {
namespace Gui {

class MusicLibraryTableView : public QTableView {
  Result setStyle() {
    setStyleSheet(
      "font-weight: 500; font-size: 12px;"
    );
    return OK;
  }

  protected:
    MusicLibraryFilters* filters;
    MusicLibraryQueryModel* model;

  public:
    MusicLibraryTableView(QWidget* parent, MusicLibraryFilters* filters, MusicLibraryQueryModel* model)
        : QTableView(parent)
        , filters(filters)
        , model(model)
        {
      setModel(model);
      setStyle();
    };

    ~MusicLibraryTableView() {
      delete model;
    }

    MusicLibraryQueryModel* getModel() const {
      return model;
    }

    Result hydrateState(const AppStatePacket& appStatePacket) const {
      model->hydrateState(appStatePacket);
      return OK;
    }

    Result refresh() const {
      model->refresh();
      return OK;
    }
};

} // Gui
} // Gj

#endif //MUSICLIBRARYTABLEVIEW_H
