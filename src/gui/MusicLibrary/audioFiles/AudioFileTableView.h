//
// Created by ns on 10/11/25.
//

#ifndef AUDIOFILETABLEVIEW_H
#define AUDIOFILETABLEVIEW_H

#include <list>

#include <caf/actor_registry.hpp>

#include "caf/actor_system.hpp"
#include "caf/scoped_actor.hpp"

#include "QObject"
#include "QThread"
#include "QStandardItem"

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
    refresh(true);
  };

  void mousePressEvent(QMouseEvent* event) override;

  void mouseDoubleClickEvent(QMouseEvent* event) override;

  Result saveCache() const;
};
} // Gui
} // Gj

#endif //AUDIOFILETABLEVIEW_H
