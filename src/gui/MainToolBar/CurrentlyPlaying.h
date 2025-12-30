//
// Created by ns on 5/17/25.
//

#ifndef CURRENTLYPLAYING_H
#define CURRENTLYPLAYING_H

#include "caf/actor_system.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QWidget>
#include <QScrollArea>
#include <QScrollBar>

#include "../../state/AppState.h"
#include "../../audio/mixer/Core.h"

namespace Gj {
namespace Gui {
using namespace caf;

class CurrentlyPlaying final : public QWidget {
public:
  explicit CurrentlyPlaying(QWidget* parent, actor_system& actorSystem, Audio::Mixer::Core* mixer);

  void hydrateState(const AppStatePacket& appStatePacket) const;

private:
  actor_system& actorSystem;
  Audio::Mixer::Core* mixer;
  QLabel* track;
  QScrollArea* trackScrollArea;
  QLabel* album;
  QScrollArea* albumScrollArea;
  QLabel* artist;
  QScrollArea* artistScrollArea;
  QGridLayout grid;

  void setupGrid();

  void setStyle();
};
} // Gui
} // Gj

#endif //CURRENTLYPLAYING_H
