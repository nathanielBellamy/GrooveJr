//
// Created by ns on 3/8/25.
//

#include "ReplaceEffectButton.h"

namespace Gj {
namespace Gui {

ReplaceEffectButton::ReplaceEffectButton(QWidget *parent, int channelIndex, int slotIndex, bool occupied, QAction* action)
  : QPushButton("", parent)
  , channelIndex(channelIndex)
  , slotIndex(slotIndex)
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
  replaceEffectAction->setData(QVariant(slotIndex));
  replaceEffectAction->activate(QAction::Trigger);
}

void ReplaceEffectButton::setStyle() {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  setMinimumSize(QSize(20, 20));
  setStyleSheet("padding: 2px;");
}

} // Gui
} // Gj