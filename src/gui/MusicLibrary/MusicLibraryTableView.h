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
#include "../../db/Dao.h"

#include "MusicLibraryQueryModel.h"
#include "MusicLibraryFilters.h"

namespace Gj {
namespace Gui {

class MusicLibraryTableView : public QTableView {
  QMenu* menu;
  Db::Dao* dao;
  Result setStyle() {
    setStyleSheet(
      "font-weight: 500; font-size: 12px;"
    );
    return OK;
  }

  protected:
    MusicLibraryType type;
    size_t idCol;
    MusicLibraryFilters* filters;
    MusicLibraryQueryModel* model;

  public:
    MusicLibraryTableView(
      QWidget* parent,
      Db::Dao* dao,
      const MusicLibraryType type,
      const size_t idCol,
      MusicLibraryFilters* filters,
      MusicLibraryQueryModel* model)
        : QTableView(parent)
        , menu(nullptr)
        , dao(dao)
        , type(type)
        , idCol(idCol)
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

  void mousePressEvent(QMouseEvent* event) override {
      if (event->button() == Qt::RightButton) {
        if (const QModelIndex index = indexAt(event->pos()); index.isValid()) {
          selectRow(index.row());

          delete menu;
          menu = new QMenu(this);
          const QAction* addToQueueAction = menu->addAction("Add To Queue");
          connect(addToQueueAction, &QAction::triggered, this, [&]() {
            const QVariant id = getModel()->index(index.row(), idCol).data();

            std::cout << "add to queue action type: " << type << " id: " << id.toString().toStdString() << std::endl;
            const Db::Queue q(id.toLongLong(), 0);
            dao->queueRepository.save(q);

            refresh();
          });
          menu->addAction("Edit");

          menu->exec(viewport()->mapToGlobal(event->pos()));
          return; // don't call base class handler
        }
      }

      QTableView::mousePressEvent(event); // default behavior
    }
};

} // Gui
} // Gj

#endif //MUSICLIBRARYTABLEVIEW_H
