//
// Created by ns on 10/11/25.
//

#ifndef AUDIOFILETABLEVIEW_H
#define AUDIOFILETABLEVIEW_H

#include <list>

#include <caf/actor_registry.hpp>

#include "caf/actor_system.hpp"
#include "caf/scoped_actor.hpp"

#include <QTimer>

#include "QObject"
#include "QThread"
#include "QStandardItem"
#include <QTimer>

#include "../../../Logging.h"
#include "../../../messaging/atoms.h"
#include "../../../messaging/DecksState.h"
#include "../../../actors/ActorIds.h"
#include "../../../enums/Result.h"
#include "../../../state/Core.h"
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
  AudioFileTableView(
    QWidget* parent,
    actor_system& actorSystem,
    Gj::State::Core* stateCore,
    Db::Dao* dao,
    MusicLibraryFilters* filters,
    SqlWorkerPool* sqlWorkerPool
  )
  : MusicLibraryTableView(
    parent,
    actorSystem,
    dao,
    stateCore,
    new AudioFileQueryModel(parent, stateCore, filters, sqlWorkerPool),
    filters
  ) {
    connect(model, &QAbstractItemModel::modelReset, this, [this]() {
      QTimer::singleShot(0, this, [this]() {
        setColumnHidden(AUDIO_FILE_COL_PATH, true);
        setColumnHidden(AUDIO_FILE_COL_ID, true);

        horizontalHeader()->resizeSection(AUDIO_FILE_COL_TRACK, 200);
        horizontalHeader()->resizeSection(AUDIO_FILE_COL_ARTIST, 150);
        horizontalHeader()->resizeSection(AUDIO_FILE_COL_ALBUM, 150);
        horizontalHeader()->resizeSection(AUDIO_FILE_COL_TRACK_NUMBER, 50);
        horizontalHeader()->resizeSection(AUDIO_FILE_COL_YEAR, 75);
        horizontalHeader()->resizeSection(AUDIO_FILE_COL_GENRE, 100);
      });
    });
    refresh(true);
  };

  void mousePressEvent(QMouseEvent* event) override;

  void mouseDoubleClickEvent(QMouseEvent* event) override;

  Result saveCache() const;
};
} // Gui
} // Gj

#endif //AUDIOFILETABLEVIEW_H
