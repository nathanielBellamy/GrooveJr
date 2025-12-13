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

#include "../../../../AppState.h"
#include "../../../../Logging.h"

namespace Gj {
namespace Gui {
class ReplacePluginButton final : public QPushButton {
public:
  ReplacePluginButton(QWidget* parent, ChannelIndex channelIndex, PluginIndex pluginIndex, bool occupied,
                      QAction* action);

  ~ReplacePluginButton() override;

  void hydrateState(const AppStatePacket& appState, int newChannelIdx);

private:
  ChannelIndex channelIndex;
  PluginIndex pluginIndex;
  bool occupied;
  QAction* replacePluginAction;

  void mousePressEvent(QMouseEvent* event) override;

  void setStyle();
};
} // Gui
} // Gj


#endif //ADDSWAPBUTTON_H
