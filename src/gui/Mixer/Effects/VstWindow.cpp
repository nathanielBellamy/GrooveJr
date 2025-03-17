//
// Created by ns on 3/16/25.
//

#include "VstWindow.h"

namespace Gj {
namespace Gui {

VstWindow::VstWindow(QWidget* parent)
  : QWidget(parent)
  {}

void VstWindow::show() {
 // TODO
}

void VstWindow::close() {
  // TODO
}

void VstWindow::resize(Size newSize) {
  // TODO
}

Size VstWindow::getContentSize() {
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