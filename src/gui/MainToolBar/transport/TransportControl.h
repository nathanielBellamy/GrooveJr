//
// Created by ns on 11/17/24.
//

#ifndef TRANSPORTCONTROL_H
#define TRANSPORTCONTROL_H

#include "caf/actor_registry.hpp"
#include "caf/actor_system.hpp"
#include "caf/scoped_actor.hpp"

#include <QAction>
#include <QGridLayout>
#include <QLabel>
#include <QStyle>
#include <QWidget>

#include "../../../Logging.h"
#include "../../../actors/ActorIds.h"
#include "../../../messaging/atoms.h"
#include "../../../messaging/EnvelopeQtPtr.h"
#include "../../../enums/PlayState.h"
#include "../../../audio/mixer/Core.h"
#include "../../Color.h"

#include "TransportControlButton.h"
#include "ProgressBar.h"
#include "PlaybackSpeedSlider.h"

namespace Gj {
namespace Gui {
using namespace caf;

class TransportControl final : public QWidget {
public:
  TransportControl(QWidget* parent, actor_system& sys, Audio::Mixer::Core* mixer);

  ~TransportControl();

  int hydrateState(const State::Packet& statePacket);

  void setPlayState(PlayState state);

private:
  actor_system& sys;
  PlayState playState;

  QGridLayout* grid;
  ProgressBar* progressBar;
  PlaybackSpeedSlider* playbackSpeedSlider;

  QAction* playTrigAction;
  QAction* pauseTrigAction;
  QAction* stopTrigAction;
  QAction* rwTrigAction;
  QAction* ffTrigAction;
  TransportControlButton* playButton;
  TransportControlButton* pauseButton;
  TransportControlButton* stopButton;
  TransportControlButton* rwButton;
  TransportControlButton* ffButton;

  void connectActions();

  void setupGrid();
};
} // Gui
} // Gj

#endif //TRANSPORTCONTROL_H
