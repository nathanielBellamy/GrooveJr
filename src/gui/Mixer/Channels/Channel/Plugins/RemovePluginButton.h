//
// Created by ns on 4/30/25.
//

#ifndef REMOVEEFFECTBUTTON_H
#define REMOVEEFFECTBUTTON_H

#include <QAction>
#include <QIcon>
#include <QPoint>
#include <QPushButton>
#include <QStyle>
#include <QVariant>

#include "../../../../../AppState.h"

namespace Gj {
namespace Gui {
namespace Mixer {
class RemovePluginButton final : public QPushButton {
public:
  RemovePluginButton(QWidget* parent, ChannelIndex channelIndex, PluginIndex pluginIndex, bool occupied,
                     QAction* action);

  void hydrateState(const AppStatePacket& appState, ChannelIndex newChannelIdx);

private:
  ChannelIndex channelIndex;
  PluginIndex pluginIndex;
  bool occupied;
  QAction* removePluginAction;

  void mousePressEvent(QMouseEvent* event) override;

  void setStyle();
};
} // Mixer
} // Gui
} // Gj

#endif //REMOVEEFFECTBUTTON_H
