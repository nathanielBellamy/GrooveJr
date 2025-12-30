//
// Created by ns on 5/18/25.
//

#ifndef MAINTOOLBAR_H
#define MAINTOOLBAR_H

#include "caf/actor_registry.hpp"
#include "caf/actor_system.hpp"
#include "caf/scoped_actor.hpp"

#include <QToolbar>
#include <QAction>
#include <QLabel>
#include <QStyle>
#include <QWidget>

#include "../../Logging.h"
#include "../../state/AppState.h"
#include "../../actors/ActorIds.h"
#include "../../messaging/atoms.h"
#include "../../messaging/EnvelopeQtPtr.h"
#include "../../enums/PlayState.h"
#include "../../audio/mixer/Core.h"
#include "../Color.h"
#include "../QSql/SqlWorkerPool.h"

#include "CurrentlyPlaying.h"
#include "eq/EqGraph.h"
#include "transport/TransportControl.h"
#include "scenes/Scenes.h"

namespace Gj {
namespace Gui {
using namespace caf;

class MainToolBar final : public QToolBar {
public:
  MainToolBar(
    QWidget* parent,
    actor_system& sys,
    AppState* gAppState,
    Audio::Mixer::Core* mixer,
    SqlWorkerPool* sqlWorkerPool,
    QAction* sceneLoadAction
  );

  int hydrateState(const AppStatePacket& appStatePacket);

private:
  actor_system& sys;
  QLabel title;
  Scenes scenes;
  CurrentlyPlaying currentlyPlaying;
  TransportControl transportControl;
  EqGraph eqGraph;
};
} // Gui
} // Gj

#endif //MAINTOOLBAR_H
