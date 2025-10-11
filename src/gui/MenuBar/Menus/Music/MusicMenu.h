//
// Created by ns on 2/25/25.
//

#ifndef MUSICMENU_H
#define MUSICMENU_H

#include "caf/actor_from_state.hpp"
#include "caf/actor_ostream.hpp"
#include "caf/actor_registry.hpp"
#include "caf/actor_system.hpp"
#include "caf/caf_main.hpp"
#include "caf/event_based_actor.hpp"
#include "caf/scoped_actor.hpp"

#include <QMenu>
#include <QAction>
#include <QWidget>
#include <QList>
#include <QUrl>
#include <QString>

#include "../../../../actors/ActorIds.h"
#include "../../../../messaging/atoms.h"
#include "../../../../enums/Result.h"

#include "../../../../Logging.h"

#include "./FolderSelect.h"

namespace Gj {
namespace Gui {

using namespace caf;

class MusicMenu final : public QMenu {

  actor_system& actorSystem;
  QAction addFolderToLibraryAction;
  FolderSelect folderSelect;
  QUrl folderUrl;

  public:
    MusicMenu(actor_system& actorSystem, QWidget* parent);
};

} // Gui
} // Gj

#endif //MUSICMENU_H
