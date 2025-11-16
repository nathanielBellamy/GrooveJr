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

#include "../../../AppState.h"
#include "../../../enums/Result.h"
#include "../../../audio/Mixer.h"
#include "../../../actors/ActorIds.h"
#include "../../../messaging/atoms.h"

#include "ScenesTableView.h"
#include "SceneSaveButton.h"

namespace Gj {
namespace Gui {

using namespace caf;

class Scenes final : public QWidget {
  actor_system& sys;
  AppState* gAppState;
  Audio::Mixer* mixer;
  QGridLayout grid;
  QLabel title;
  ScenesTableView tableView;
  QAction sceneSaveAction;
  SceneSaveButton sceneSaveButton;

  void connectActions();
  void setupGrid();

  public:
    Scenes(QWidget* parent, actor_system& sys, AppState* gAppState, Audio::Mixer* mixer, QAction* sceneLoadAction);
    Result hydrateState(const AppStatePacket& appStatePacket);
};


} // Gui
} // Gj

#endif //SCENESELECT_H
