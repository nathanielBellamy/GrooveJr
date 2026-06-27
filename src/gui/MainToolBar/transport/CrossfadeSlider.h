//
// Created by ns on 6/26/26.
//

#ifndef CROSSFADESLIDER_H
#define CROSSFADESLIDER_H

#include <QLabel>
#include <QSlider>

#include "../../../audio/mixer/Core.h"

namespace Gj {
namespace Gui {
class CrossfadeSlider final : public QSlider {
  Audio::Mixer::Core* mixer;

public:
  CrossfadeSlider(QWidget* parent, Audio::Mixer::Core* mixer, QLabel* label);

  void mouseDoubleClickEvent(QMouseEvent* event) override;
};
} // Gui
} // Gj


#endif //CROSSFADESLIDER_H
