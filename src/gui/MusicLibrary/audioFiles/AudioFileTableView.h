//
// Created by ns on 10/11/25.
//

#ifndef AUDIOFILETABLEVIEW_H
#define AUDIOFILETABLEVIEW_H

#include <list>

#include <caf/actor_registry.hpp>

#include "caf/actor_system.hpp"
#include "caf/scoped_actor.hpp"

#include "../../../Logging.h"
#include "../../../messaging/atoms.h"
#include "../../../actors/ActorIds.h"
#include "../../../enums/Result.h"
#include "../../../AppState.h"


#include "../MusicLibraryFilters.h"
#include "../MusicLibraryTableView.h"
#include "../MusicLibraryType.h"
#include "AudioFileQueryModel.h"

namespace Gj {
namespace Gui {

using namespace caf;

class AudioFileTableView final : public MusicLibraryTableView {

  AppState* gAppState;

public:
  AudioFileTableView(QWidget* parent, actor_system& actorSystem, AppState* gAppState, Db::Dao* dao, MusicLibraryFilters* filters)
      : MusicLibraryTableView(parent, actorSystem, dao, filters, new AudioFileQueryModel(parent, filters))
      , gAppState(gAppState)
      {};

  Result refresh() const {
    std::list<Db::ID> audioFileIdCache {};

    int i = 0;
    auto index = model->index(0, AUDIO_FILE_COL_ID);

    while (index.isValid()) {
      audioFileIdCache.push_back(index.data().toULongLong());
      i++;
      index = model->index(i, AUDIO_FILE_COL_ID);
    }

    MusicLibraryTableView::refresh();
    return OK;
  }

  void mousePressEvent(QMouseEvent* event) override {
    if (event->button() == Qt::RightButton) {
      if (const QModelIndex index = indexAt(event->pos()); index.isValid()) {
        selectRow(index.row());

        delete menu;
        menu = new QMenu(this);
        const QAction* addToQueueAction = menu->addAction("Add To Queue");
        connect(addToQueueAction, &QAction::triggered, this, [&]() {
          const QVariant id = getModel()->index(index.row(), AUDIO_FILE_COL_ID).data();
          const Db::Queue q(id.toULongLong(), 0);

          if (const auto queueId = dao->queueRepository.save(q); queueId > 0) {
            Logging::write(
              Info,
              "Gui::AudioFileTableView::mousePressEvent::addToQueueAction",
              "Saved AudioFileId " + std::to_string(q.audioFileId) + " to Queue with QueueId: " + std::to_string(queueId)
            );
          } else {
            Logging::write(
              Error,
              "Gui::AudioFileTableView::mousePressEvent::addToQueueAction",
              "Unable to save AudioFileId " + std::to_string(q.audioFileId) + " to Queue."
            );
          }

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
      const MusicLibraryQueryModel* model = getModel();
      const QVariant id = model->index(index.row(), AUDIO_FILE_COL_ID).data();

      int i = 0;
      while (model->index(i, AUDIO_FILE_COL_ID).isValid()) {
        const auto audioFileId = model->index(i, AUDIO_FILE_COL_ID).data().toULongLong();
        // TODO:
        // - save cache
        i++;
      }

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
