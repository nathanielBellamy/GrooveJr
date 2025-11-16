//
// Created by ns on 11/5/25.
//

#include "AudioFileTableView.h"

namespace Gj {
namespace Gui {

void AudioFileTableView::mousePressEvent(QMouseEvent* event) {
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

Result AudioFileTableView::saveCache() const {
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


void AudioFileTableView::mouseDoubleClickEvent(QMouseEvent *event) {
  if (const QModelIndex clickedIndex = indexAt(event->pos()); clickedIndex.isValid()) {
    const SqlQueryModel* model = getModel();
    const int clickedRow = clickedIndex.row();
    DecksState decksState;
    if (clickedRow < 1) {
      decksState.currentDeckIdx = 0;
      for (int i = 0; i < Audio::AUDIO_CORE_DECK_COUNT; i++) {
        decksState.audioFileIds[i] = model->index(clickedRow + i, AUDIO_FILE_COL_ID).data().toULongLong();
      }
      const Db::ID id = model->index(clickedIndex.row(), AUDIO_FILE_COL_ID).data().toULongLong();
    } else {
      decksState.currentDeckIdx = 1;
      for (int i = 0; i < Audio::AUDIO_CORE_DECK_COUNT; i++) {
        decksState.audioFileIds[i] = model->index(clickedRow + i - 1, AUDIO_FILE_COL_ID).data().toULongLong();
      }
    }

    if (saveCache() == ERROR) {
      Logging::write(
        Error,
        "Gui::AudioFileTableView::mouseDoubleClickEvent",
        "Unable to save Caches."
      );
      return;
    }

    if (gAppState->getCurrentlyPlaying().audioFile.id == decksState.audioFileIds[decksState.currentDeckIdx]
          && !gAppState->queuePlay)
      return;

    const auto appStateManagerPtr = actorSystem.registry().get(Act::ActorIds::APP_STATE_MANAGER);
    const scoped_actor self{ actorSystem };
    self->anon_send(
        actor_cast<actor>(appStateManagerPtr),
        false, // queuePlay
        decksState,
        tc_trig_play_file_a_v
    );
  }
};

} // Gui
} // Gj