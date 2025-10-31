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
#include "../../../db/entity/musicLibrary/Cache.h"

#include "../MusicLibraryFilters.h"
#include "../MusicLibraryTableView.h"
#include "../MusicLibraryType.h"
#include "AudioFileQueryModel.h"

namespace Gj {
namespace Gui {

using namespace caf;

class AudioFileTableView final : public MusicLibraryTableView {

public:
  AudioFileTableView(QWidget* parent, actor_system& actorSystem, AppState* gAppState, Db::Dao* dao, MusicLibraryFilters* filters)
      : MusicLibraryTableView(parent, actorSystem, dao, gAppState, filters, new AudioFileQueryModel(parent, gAppState, filters))
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

  Result saveCache() const {
    std::vector<Db::Cache> caches;
    int i = 0;
    while (model->index(i, AUDIO_FILE_COL_ID).isValid()) {
      const auto audioFileId = model->index(i, AUDIO_FILE_COL_ID).data().toULongLong();
      const Db::Cache cache(audioFileId);
      caches.push_back(cache);
      i++;
    }
    if (dao->cacheRepository.save(caches) == ERROR) {
      Logging::write(
        Error,
        "Gui::AudioFileTableView::saveCache",
        "Unable to save Caches."
      );
      return ERROR;
    }

    Logging::write(
      Info,
      "Gui::AudioFileTableView::saveCache",
      "Saved Caches. Count: " + std::to_string(caches.size())
    );
    return OK;
  }

  void mouseDoubleClickEvent(QMouseEvent *event) override {
    if (const QModelIndex clickedIndex = indexAt(event->pos()); clickedIndex.isValid()) {
      const MusicLibraryQueryModel* model = getModel();
      const Db::ID id = model->index(clickedIndex.row(), AUDIO_FILE_COL_ID).data().toULongLong();

      if (saveCache() == ERROR) {
        Logging::write(
          Error,
          "Gui::AudioFileTableView::mouseDoubleClickEvent",
          "Unable to save Caches."
        );
        return;
      }

      if (gAppState->getCurrentlyPlaying().audioFile.id == id && !gAppState->queuePlay)
        return;

      gAppState->queuePlay = false;
      const std::optional<Db::DecoratedAudioFile> decoratedAudioFile =
        dao->audioFileRepository.findDecoratedAudioFileById(id);

      if (!decoratedAudioFile) {
        Logging::write(
          Error,
          "Gui::AudioFileTableView::mouseDoubleClickEvent",
          "Unable to load DecoratedAudioFile Id: " + std::to_string(id)
        );
        return;
      }

      const auto appStateManagerPtr = actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER);
      const scoped_actor self{ actorSystem };
      self->anon_send(
          actor_cast<actor>(appStateManagerPtr),
          id,
          tc_trig_play_file_a_v
      );

      gAppState->setCurrentlyPlaying(decoratedAudioFile.value());
      refresh();
    }
  };
};

} // Gui
} // Gj

#endif //AUDIOFILETABLEVIEW_H
