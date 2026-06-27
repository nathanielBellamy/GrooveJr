//
// Created by ns on 7/4/25.
//

#ifndef PLAYBACKSPEEDSLIDER_H
#define PLAYBACKSPEEDSLIDER_H

#include <QLabel>
#include <QSlider>

#include "../../../audio/mixer/Core.h"

namespace Gj {
namespace Gui {
class PlaybackSpeedSlider final : public QSlider {
  Audio::Mixer::Core* mixer;

public:
  PlaybackSpeedSlider(QWidget* parent, Audio::Mixer::Core* mixer, QLabel* label);

  void mouseDoubleClickEvent(QMouseEvent* event) override;
};
} // Gui
} // Gj

#endif //PLAYBACKSPEEDSLIDER_H
