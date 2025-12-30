//
// Created by ns on 3/8/25.
//

#ifndef ADDSWAPBUTTON_H
#define ADDSWAPBUTTON_H

#include <QAction>
#include <QIcon>
#include <QPoint>
#include <QPushButton>
#include <QStyle>
#include <QVariant>

#include "../../../../../state/Core.h"
#include "../../../../../Logging.h"

namespace Gj {
namespace Gui {
namespace Mixer {
class ReplacePluginButton final : public QPushButton {
public:
  ReplacePluginButton(QWidget* parent, ChannelIndex channelIndex, PluginIndex pluginIndex, bool occupied,
                      QAction* action);

  ~ReplacePluginButton() override;

  void hydrateState(const State::Packet& appState, int newChannelIdx);

private:
  ChannelIndex channelIndex;
  PluginIndex pluginIndex;
  bool occupied;
  QAction* replacePluginAction;

  void mousePressEvent(QMouseEvent* event) override;

  void setStyle();
};
} // Mixer
} // Gui
} // Gj


#endif //ADDSWAPBUTTON_H
