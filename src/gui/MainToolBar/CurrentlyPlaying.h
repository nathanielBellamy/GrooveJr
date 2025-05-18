//
// Created by ns on 5/17/25.
//

#ifndef CURRENTLYPLAYING_H
#define CURRENTLYPLAYING_H

#include "caf/actor_system.hpp"

#include <QGridLayout>
#include <QLabel>
#include <QWidget>

#include "../../AppState.h"
#include "../../audio/Mixer.h"

namespace Gj {
namespace Gui {

using namespace caf;

class CurrentlyPlaying final : public QWidget {
  public:
    explicit CurrentlyPlaying(QWidget *parent, actor_system& actorSystem, Audio::Mixer* mixer);
    void hydrateState(const AppStatePacket& appStatePacket);

  private:
    actor_system& actorSystem;
    Audio::Mixer* mixer;
    QLabel track;
    QLabel album;
    QLabel artist;
    QGridLayout grid;
    void setStyle();
};

} // Gui
} // Gj

#endif //CURRENTLYPLAYING_H
