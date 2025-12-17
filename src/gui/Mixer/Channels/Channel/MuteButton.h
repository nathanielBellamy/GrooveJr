//
// Created by ns on 3/8/25.
//

#ifndef MUTEBUTTON_H
#define MUTEBUTTON_H

#include <QAction>
#include <QPushButton>

#include "../../../Color.h"

namespace Gj {
namespace Gui {
namespace Mixer {
class MuteButton final : public QPushButton {
public:
  MuteButton(QWidget* parent, QAction* muteChannelAction, int channelIndex);

  void setMute(float val);

private:
  int channelIndex;
  QAction* muteChannelAction;

  std::string styleString(float muteVal) const;

  void setStyle();

  void mousePressEvent(QMouseEvent* event) override;
};
} // Mixer
} // Gui
} // Gj


#endif //MUTEBUTTON_H
