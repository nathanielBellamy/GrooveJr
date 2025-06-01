//
// Created by ns on 6/1/25.
//

#ifndef SCENESELECT_H
#define SCENESELECT_H

#include "caf/actor_registry.hpp"
#include "caf/actor_system.hpp"
#include "caf/scoped_actor.hpp"

#include <QAction>
#include <QWidget>
#include <QGridLayout>

#include "../../audio/Mixer.h"
#include "../../actors/ActorIds.h"
#include "../../messaging/atoms.h"

namespace Gj {
namespace Gui {

using namespace caf;

class SceneSelect final : public QWidget {


  public:
    SceneSelect(QWidget* parent, actor_system& sys, Audio::Mixer* mixer);
};


} // Gui
} // Gj

#endif //SCENESELECT_H
