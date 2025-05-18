//
// Created by ns on 5/17/25.
//

#include "CurrentlyPlaying.h"

namespace Gj {
namespace Gui {

CurrentlyPlaying::CurrentlyPlaying(QWidget* parent, actor_system& actorSystem, Audio::Mixer* mixer)
  : QWidget(parent)
  , actorSystem(actorSystem)
  , mixer(mixer)
  , title(this)
  , grid(this)
  {

  title.setText("CurrPlay");
  title.setFont({title.font().family(), 36});
  grid.addWidget(&title, 0, 0, -1, -1);
  setLayout(&grid);

  setStyle();
}

void CurrentlyPlaying::setStyle() {
  setStyleSheet("background-color: blue;");
}

} // Gui
} // Gj