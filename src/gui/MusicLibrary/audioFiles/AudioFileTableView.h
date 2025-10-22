//
// Created by ns on 10/11/25.
//

#ifndef AUDIOFILETABLEVIEW_H
#define AUDIOFILETABLEVIEW_H

#include <caf/actor_registry.hpp>

#include "caf/actor_system.hpp"
#include "caf/scoped_actor.hpp"
#include "../../../../messaging/atoms.h"
#include "../../../../actors/ActorIds.h"

#include "../MusicLibraryFilters.h"
#include "../MusicLibraryTableView.h"
#include "../MusicLibraryType.h"
#include "AudioFileQueryModel.h"

namespace Gj {
namespace Gui {

class AudioFileTableView final : public MusicLibraryTableView {

  public:
    AudioFileTableView(QWidget* parent, actor_system& actorSystem, Db::Dao* dao, MusicLibraryFilters* filters)
        : MusicLibraryTableView(parent, actorSystem, dao, filters, new AudioFileQueryModel(parent, filters))
        {};



  void mousePressEvent(QMouseEvent* event) override {
      if (event->button() == Qt::RightButton) {
        if (const QModelIndex index = indexAt(event->pos()); index.isValid()) {
          selectRow(index.row());

          delete menu;
          menu = new QMenu(this);
          const QAction* addToQueueAction = menu->addAction("Add To Queue");
          connect(addToQueueAction, &QAction::triggered, this, [&]() {
            const QVariant id = getModel()->index(index.row(), AUDIO_FILE_COL_ID).data();

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
    };

    void mouseDoubleClickEvent(QMouseEvent *event) override {
      if (const QModelIndex index = indexAt(event->pos()); index.isValid()) {
        // selectRow(index.row());
        const QVariant id = getModel()->index(index.row(), AUDIO_FILE_COL_ID).data();

        const auto appStateManagerPtr = actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER);

        const scoped_actor self{ actorSystem };
        self->anon_send(
            actor_cast<actor>(appStateManagerPtr),
            id.toULongLong(),
            tc_trig_play_file_a_v
        );
      }
    };
};

} // Gui
} // Gj

#endif //AUDIOFILETABLEVIEW_H
