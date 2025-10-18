//
// Created by ns on 10/17/25.
//

#ifndef MUSICLIBRARYTABLEVIEW_H
#define MUSICLIBRARYTABLEVIEW_H

#include <QTableView>

#include "../../enums/Result.h"
#include "MusicLibraryQueryModel.h"
#include "../../AppState.h"
#include "MusicLibraryFilters.h"

namespace Gj {
namespace Gui {

class MusicLibraryTableView : public QTableView {
  MusicLibraryQueryModel* model;
  Result setStyle() {
    setStyleSheet(
      "font-weight: 500; font-size: 12px;"
    );
    return OK;
  }

  public:
    MusicLibraryTableView(QWidget* parent, MusicLibraryFilters* filters, MusicLibraryType type)
        : QTableView(parent)
        {
      setStyle();
    };

    ~MusicLibraryTableView() {
      delete model;
    }

    virtual Result hydrateState(const AppStatePacket& appStatePacket) = 0;

    Result refresh() {
      model->refresh();
      return OK;
    }
};

} // Gui
} // Gj

#endif //MUSICLIBRARYTABLEVIEW_H
