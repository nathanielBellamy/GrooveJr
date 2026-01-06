//
// Created by ns on 6/1/25.
//

#ifndef SCENESELECT_H
#define SCENESELECT_H

#include "caf/actor_registry.hpp"
#include "caf/actor_system.hpp"
#include "caf/scoped_actor.hpp"

#include <QAction>
#include <QLabel>
#include <QWidget>
#include <QGridLayout>

#include "../../../state/Core.h"
#include "../../../enums/Result.h"
#include "../../../audio/mixer/Core.h"
#include "../../../actors/ActorIds.h"
#include "../../../messaging/atoms.h"

#include "ScenesTableView.h"
#include "SceneButton.h"

namespace Gj {
namespace Gui {
using namespace caf;

class Scenes final : public QWidget {
  actor_system& sys;
  State::Core* stateCore;
  Audio::Mixer::Core* mixer;
  QGridLayout* grid;
  QLabel* title;
  ScenesTableView* tableView = nullptr;
  QAction* sceneNewAction;
  SceneButton* sceneNewButton;
  QAction* sceneSaveAction;
  SceneButton* sceneSaveButton;
  QAction* sceneLoadAction;

  void connectActions();

  void setupGrid();

  Result setStyle();

public:
  Scenes(
    QWidget* parent,
    actor_system& sys,
    State::Core* stateCore,
    Audio::Mixer::Core* mixer,
    SqlWorkerPool* sqlWorkerPool,
    QAction* sceneLoadAction
  );

  ~Scenes();

  Result hydrateState(const State::Packet& statePacket);
};
} // Gui
} // Gj

#endif //SCENESELECT_H
