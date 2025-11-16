//
// Created by ns on 10/11/25.
//

#ifndef QUEUETABLEVIEW_H
#define QUEUETABLEVIEW_H

#include "../../../AppState.h"
#include "../MusicLibraryType.h"
#include "../MusicLibraryFilters.h"
#include "../MusicLibraryTableView.h"
#include "QueueQueryModel.h"

namespace Gj {
namespace Gui {

class QueueTableView final : public MusicLibraryTableView {

  public:
    QueueTableView(
      QWidget* parent,
      actor_system& actorSystem,
      Db::Dao* dao,
      AppState* gAppState,
      MusicLibraryFilters* filters,
      SqlWorkerPool* sqlWorkerPool
    )
    : MusicLibraryTableView(
        parent,
        actorSystem,
        dao,
        gAppState,
        new QueueQueryModel(parent, gAppState, filters, sqlWorkerPool),
        filters
      )
    {};

  void mouseDoubleClickEvent(QMouseEvent *event) override {
    if (const QModelIndex clickedIndex = indexAt(event->pos()); clickedIndex.isValid()) {
      const SqlQueryModel* model = getModel();
      const Db::ID id = model->index(clickedIndex.row(), AUDIO_FILE_COL_ID).data().toULongLong();

      if (gAppState->getCurrentlyPlaying().audioFile.id == id && !gAppState->queuePlay)
        return;

      gAppState->queuePlay = true;
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
  }
};

} // Gui
} // Gj

#endif //QUEUETABLEVIEW_H
