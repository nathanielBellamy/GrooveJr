//
// Created by ns on 7/4/25.
//

#include "PlaybackSpeedSlider.h"

namespace Gj {
namespace Gui {
PlaybackSpeedSlider::PlaybackSpeedSlider(QWidget* parent, Audio::Mixer::Core* mixer)
: QSlider(Qt::Horizontal, parent)
  , mixer(mixer) {
  setMinimum(-200);
  setMaximum(200);
  setTickInterval(1);
  setValue(mixer->getPlaybackSpeed());
  setTickPosition(NoTicks);

  auto connection = connect(this, &QSlider::valueChanged, [this](const int newPlaybackSpeed) {
    this->mixer->setPlaybackSpeed(newPlaybackSpeed);
  });
}

void PlaybackSpeedSlider::mouseDoubleClickEvent(QMouseEvent* event) {
  setValue(100);
}
} // Gui
} // Gj
