//
// Created by ns on 11/17/24.
//

#ifndef TRANSPORTCONTROL_H
#define TRANSPORTCONTROL_H

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

namespace Gj {
namespace Gui {

using namespace caf;

class TransportControl final : public QToolBar {
  public:
    TransportControl(QWidget* parent, actor_system& sys, Audio::Mixer* mixer);
    int hydrateState(const AppStatePacket& appStatePacket);
    void setPlayState(PlayState state);

  private:
    actor_system& sys;
    PlayState playState;
    QLabel title;
    CurrentlyPlaying currentlyPlaying;
    QAction playTrigAction {style()->standardIcon(QStyle::StandardPixmap::SP_MediaPlay), "", this};
    QAction pauseTrigAction {style()->standardIcon(QStyle::StandardPixmap::SP_MediaPause), "", this};
    QAction stopTrigAction {style()->standardIcon(QStyle::StandardPixmap::SP_MediaStop), "", this};
    QAction rwTrigAction {style()->standardIcon(QStyle::StandardPixmap::SP_MediaSkipBackward), "", this};
    QAction ffTrigAction {style()->standardIcon(QStyle::StandardPixmap::SP_MediaSkipForward), "", this};
};


} // Gui
} // Gj

#endif //TRANSPORTCONTROL_H
