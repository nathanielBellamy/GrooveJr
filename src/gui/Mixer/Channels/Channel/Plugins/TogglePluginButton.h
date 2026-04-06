//
// Created by ns on 4/5/26.
//

#ifndef TOGGLEPLUGINBUTTON_H
#define TOGGLEPLUGINBUTTON_H

#include <QAction>
#include <QIcon>
#include <QPoint>
#include <QPushButton>
#include <QStyle>
#include <QVariant>

#include "../../../../../state/Core.h"

namespace Gj {
namespace Gui {
namespace Mixer {
class TogglePluginButton final : public QPushButton {
public:
  TogglePluginButton(QWidget* parent, ChannelIndex channelIndex, PluginIndex pluginIndex, bool occupied,
                      QAction* action);

  void hydrateState(const State::Packet& appState, ChannelIndex newChannelIdx);

private:
  ChannelIndex channelIndex;
  PluginIndex pluginIndex;
  bool occupied;
  QAction* togglePluginAction;

  void mousePressEvent(QMouseEvent* event) override;

  void setStyle();
};
} // Mixer
} // Gui
} // Gj


#endif //TOGGLEPLUGINBUTTON_H
