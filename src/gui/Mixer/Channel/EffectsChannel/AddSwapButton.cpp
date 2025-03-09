//
// Created by ns on 3/8/25.
//

#include "AddSwapButton.h"

namespace Gj {
namespace Gui {

AddSwapButton::AddSwapButton(QWidget *parent, int channelIndex, int slotIndex, bool occupied, QAction* action)
  : QPushButton(occupied ? "&Swap" : "&Add", parent)
  , channelIndex(channelIndex)
  , slotIndex(slotIndex)
  , occupied(occupied)
  , addEffectAction(action)
  {}

void AddSwapButton::mousePressEvent(QMouseEvent* event) {
  std::cout << "AddSwapButton::hitButton : " << channelIndex << " : " << slotIndex << std::endl;
  addEffectAction->activate(QAction::Trigger);
}


} // Gui
} // Gj