//
// Created by ns on 10/17/25.
//

#ifndef MUSICLIBRARYTABLEVIEW_H
#define MUSICLIBRARYTABLEVIEW_H

#include <QMenu>
#include <QMouseEvent>
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
      setContextMenuPolicy(Qt::CustomContextMenu);
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

  void mousePressEvent(QMouseEvent* event) override {
      if (event->button() == Qt::RightButton) {
        if (const QModelIndex index = indexAt(event->pos()); index.isValid()) {
          selectRow(index.row());

          QMenu menu(this);
          menu.addAction("Add To Queue");
          menu.addAction("Edit");

          menu.exec(viewport()->mapToGlobal(event->pos()));
          return; // don't call base class handler
        }
      }

      QTableView::mousePressEvent(event); // default behavior
    }
};

} // Gui
} // Gj

#endif //MUSICLIBRARYTABLEVIEW_H
