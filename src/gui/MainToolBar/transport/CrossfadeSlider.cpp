//
// Created by ns on 6/26/26.
//

#include "CrossfadeSlider.h"

namespace Gj {
namespace Gui {
CrossfadeSlider::CrossfadeSlider(QWidget* parent, Audio::Mixer::Core* mixer, QLabel* label)
: QSlider(Qt::Horizontal, parent)
  , mixer(mixer) {
  setMinimum(0);
  setMaximum(600000);
  setTickInterval(1);
  setValue(mixer->getCrossfade());
  setTickPosition(NoTicks);

  auto connection = connect(this, &QSlider::valueChanged, [this, label](const int newCrossfade) {
    this->mixer->setCrossfade(newCrossfade);
    if (label)
      label->setText(QString("Crossfade: %1 frames").arg(newCrossfade));
  });
}

void CrossfadeSlider::mouseDoubleClickEvent(QMouseEvent* event) {
  setValue(0);
}
} // Gui
} // Gj
