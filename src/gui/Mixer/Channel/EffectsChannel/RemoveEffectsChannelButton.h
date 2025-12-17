//
// Created by ns on 5/3/25.
//

#ifndef REMOVEEFFECTSCHANNELBUTTON_H
#define REMOVEEFFECTSCHANNELBUTTON_H

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
namespace Mixer {
class RemoveEffectsChannelButton final : public QPushButton {
public:
  RemoveEffectsChannelButton(QWidget* parent, ChannelIndex channelIndex, QAction* action);

  ~RemoveEffectsChannelButton();

  void hydrateState(const AppStatePacket& appState, ChannelIndex newChannelIdx);

private:
  ChannelIndex channelIndex;
  bool occupied;
  QAction* removeEffectsChannelAction;

  void mousePressEvent(QMouseEvent* event) override;

  void setStyle();
};
} // Mixer
} // Gui
} // Gj

#endif //REMOVEEFFECTSCHANNELBUTTON_H
