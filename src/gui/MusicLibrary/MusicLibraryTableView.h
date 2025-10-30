//
// Created by ns on 10/17/25.
//

#ifndef MUSICLIBRARYTABLEVIEW_H
#define MUSICLIBRARYTABLEVIEW_H

#include <QMenu>
#include <QMouseEvent>
#include <QTableView>

#include "caf/actor_system.hpp"

#include "../../enums/Result.h"
#include "../../AppState.h"
#include "../../db/Dao.h"

#include "MusicLibraryQueryModel.h"
#include "MusicLibraryFilters.h"

namespace Gj {
namespace Gui {

using namespace caf;

class MusicLibraryTableView : public QTableView {
  Result setStyle() {
    setStyleSheet(
      "font-weight: 500; font-size: 12px;"
    );
    return OK;
  }

  protected:
    actor_system& actorSystem;
    Db::Dao* dao;
    AppState* gAppState;
    QMenu* menu;
    MusicLibraryFilters* filters;
    MusicLibraryQueryModel* model;

  public:
    MusicLibraryTableView(
      QWidget* parent,
      actor_system& actorSystem,
      Db::Dao* dao,
      AppState* gAppState,
      MusicLibraryFilters* filters,
      MusicLibraryQueryModel* model)
        : QTableView(parent)
        , actorSystem(actorSystem)
        , dao(dao)
        , gAppState(gAppState)
        , menu(nullptr)
        , filters(filters)
        , model(model)
        {
      setContextMenuPolicy(Qt::CustomContextMenu);
      setModel(model);
      setStyle();
    };

    ~MusicLibraryTableView() {
      delete menu;
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

  // virtual Result addToQueue(Db::ID id) = 0;
};

} // Gui
} // Gj

#endif //MUSICLIBRARYTABLEVIEW_H
