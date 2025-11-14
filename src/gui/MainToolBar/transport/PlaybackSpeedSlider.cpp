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
  setValue(std::floor(mixer->audioCore->playbackSpeed * 100.0f));
  setTickPosition(QSlider::NoTicks);
  auto connection = connect(this, &QSlider::valueChanged, [&mixer](const int newPlaybackSpeed) {
    mixer->setPlaybackSpeed(newPlaybackSpeed);
  });
}

void PlaybackSpeedSlider::mouseDoubleClickEvent(QMouseEvent* event) {
  setValue(100);
}

} // Gui
} // Gj