//
// Created by ns on 3/16/25.
//

#include "VST3Window.h"

namespace Gj {
namespace Gui {

VST3Window::VST3Window(QWidget* parent)
  : QWidget(parent)
  {}

void VST3Window::show() {
 // TODO
}

void VST3Window::close() {
  // TODO
}

void VST3Window::resize(Size newSize) {
  // TODO
}

Size VST3Window::getContentSize() {
  return {
    300,
    300
  };
}


Steinberg::tresult queryInterface (const Steinberg::TUID iid, void** obj) {
  return 1;
};

} // Gui
} // Gj