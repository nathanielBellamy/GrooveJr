//
// Created by ns on 3/8/25.
//

#include "ReplaceEffectButton.h"

namespace Gj {
namespace Gui {

ReplaceEffectButton::ReplaceEffectButton(
  QWidget *parent,
  const ChannelIndex channelIndex,
  const EffectIndex effectIndex,
  const bool occupied,
  QAction* action
  )
  : QPushButton("", parent)
  , channelIndex(channelIndex)
  , effectIndex(effectIndex)
  , occupied(occupied)
  , replaceEffectAction(action) {
  setIcon(style()->standardIcon(QStyle::StandardPixmap::SP_BrowserReload));
  setCursor(Qt::PointingHandCursor);
  setStyle();
}

ReplaceEffectButton::~ReplaceEffectButton() {
  Logging::write(
    Info,
    "Gui::ReplaceEffectButton::~ReplaceEffectButton",
    "Destroying ReplaceEffectButton."
  );
}


void ReplaceEffectButton::hydrateState(const AppStatePacket &appState, int newChannelIdx) {
  channelIndex = newChannelIdx;
}

void ReplaceEffectButton::mousePressEvent(QMouseEvent* event) {
  replaceEffectAction->setData(static_cast<quint64>(effectIndex));
  replaceEffectAction->activate(QAction::Trigger);
}

void ReplaceEffectButton::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setMinimumSize(QSize(20, 20));
  setStyleSheet("padding: 2px;");
}

} // Gui
} // Gj