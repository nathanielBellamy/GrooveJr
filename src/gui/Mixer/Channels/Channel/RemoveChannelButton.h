//
// Created by ns on 5/3/25.
//

#ifndef REMOVECHANNELBUTTON_H
#define REMOVECHANNELBUTTON_H

#include <QAction>
#include <QIcon>
#include <QPoint>
#include <QPushButton>
#include <QStyle>
#include <QVariant>

#include "../../../../state/AppState.h"
#include "../../../../Logging.h"

namespace Gj {
namespace Gui {
namespace Mixer {
class RemoveChannelButton final : public QPushButton {
public:
  RemoveChannelButton(QWidget* parent, ChannelIndex channelIndex, QAction* action);

  ~RemoveChannelButton();

  void hydrateState(const AppStatePacket& appState, ChannelIndex newChannelIdx);

private:
  ChannelIndex channelIndex;
  bool occupied;
  QAction* removeChannelAction;

  void mousePressEvent(QMouseEvent* event) override;

  void setStyle();
};
} // Mixer
} // Gui
} // Gj

#endif //REMOVECHANNELBUTTON_H
