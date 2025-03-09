//
// Created by ns on 3/8/25.
//

#include "AddSwapButton.h"

namespace Gj {
namespace Gui {

AddSwapButton::AddSwapButton(QWidget *parent, bool occupied)
  : QPushButton(occupied ? "&Swap" : "&Add", parent)
  , occupied(occupied)
  {}

bool AddSwapButton::hitButton(const QPoint& pos) const {
  std::cout << "AddSwapButton::hitButton" << std::endl;
  return true;
}


} // Gui
} // Gj