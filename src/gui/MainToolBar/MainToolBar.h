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
#include "../../actors/ActorIds.h"
#include "../../messaging/atoms.h"
#include "../../messaging/EnvelopeQtPtr.h"
#include "../../enums/PlayState.h"
#include "../../audio/Mixer.h"
#include "../Color.h"

#include "CurrentlyPlaying.h"
#include "transport/TransportControl.h"
#include "scenes/Scenes.h"

namespace Gj {
namespace Gui {

using namespace caf;

class MainToolBar final : public QToolBar {
  public:
    MainToolBar(QWidget* parent, actor_system& sys, Audio::Mixer* mixer);
    int hydrateState(const AppStatePacket& appStatePacket);

  private:
    actor_system& sys;
    QLabel title;
    Scenes scenes;
    CurrentlyPlaying currentlyPlaying;
    TransportControl transportControl;
};


} // Gui
} // Gj

#endif //MAINTOOLBAR_H
