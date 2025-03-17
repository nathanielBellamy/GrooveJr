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

NativePlatformWindow VstWindow::getNativePlatformWindow() const {
  #if SMTG_OS_WINDOWS
    const Steinberg::FIDString kPlatformString = Steinberg::kPlatformStringWin;
  #elif SMTG_OS_IOS
    const Steinberg::FIDString kPlatformString = Steinberg::kPlatformStringIOS;
  #elif SMTG_OS_MACOS
    const Steinberg::FIDString kPlatformString = Steinberg::kPlatformStringMac;
  #elif SMTG_OS_LINUX
    const Steinberg::FIDString kPlatformString = Steinberg::kPlatformStringLinux;
  #endif

  return {
    Steinberg::kPlatformStringMac,
    nullptr // TODO
  };
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