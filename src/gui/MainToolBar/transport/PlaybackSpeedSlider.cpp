//
// Created by ns on 7/4/25.
//

#include "PlaybackSpeedSlider.h"

namespace Gj {
namespace Gui {

PlaybackSpeedSlider::PlaybackSpeedSlider(QWidget* parent, Audio::Mixer* mixer)
  : QSlider(Qt::Horizontal, parent)
  , mixer(mixer)
  {
  setMinimum(-200);
  setMaximum(200);
  setTickInterval(1);
  setValue(100);
  setTickPosition(QSlider::NoTicks);
  auto connection = connect(this, &QSlider::valueChanged, [&mixer](const int newPlaybackSpeed) {
    mixer->setPlaybackSpeed(newPlaybackSpeed);
  });
}

} // Gui
} // Gj