//
// Created by ns on 11/17/24.
//

#ifndef TRANSPORTCONTROL_H
#define TRANSPORTCONTROL_H

#include <iostream>

#include "caf/actor_from_state.hpp"
#include "caf/actor_ostream.hpp"
#include "caf/actor_registry.hpp"
#include "caf/actor_system.hpp"
#include "caf/caf_main.hpp"
#include "caf/event_based_actor.hpp"
#include "caf/scoped_actor.hpp"

#include "../actors/ActorIds.h"
#include "../messaging/atoms.h"
#include "../messaging/EnvelopeQtPtr.h"
#include "../enums/PlayState.h"

#include <QFrame>
#include <QToolbar>
#include <QToolButton>
#include <QAction>
#include <QStyle>
#include <QWidget>

using namespace caf;

namespace Gs {
namespace Gui {

class TransportControl : public QToolBar {
  Q_OBJECT

  public:
    TransportControl(QWidget* parent, actor_system& sys);
    int hydrateState(const Gs::AppStatePacket& appStatePacket);
    void setPlayState(Gs::PlayState state);

  private:
    actor_system& sys;
    Gs::PlayState playState;
    QAction playTrigAction {style()->standardIcon(QStyle::StandardPixmap::SP_MediaPlay), "", this};
    QAction pauseTrigAction {style()->standardIcon(QStyle::StandardPixmap::SP_MediaPause), "", this};
    QAction stopTrigAction {style()->standardIcon(QStyle::StandardPixmap::SP_MediaStop), "", this};
    QAction rwTrigAction {style()->standardIcon(QStyle::StandardPixmap::SP_MediaSkipBackward), "", this};
    QAction ffTrigAction {style()->standardIcon(QStyle::StandardPixmap::SP_MediaSkipForward), "", this};
};


} // Gui
} // Gs

#endif //TRANSPORTCONTROL_H
