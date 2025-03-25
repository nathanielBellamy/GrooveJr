//
// Created by ns on 3/16/25.
//

#include "VstWindow.h"

namespace Gj {
namespace Gui {

VstWindow::VstWindow(QWidget* parent)
  : QWidget(parent)
  , grid(this)
  , title(this) {

  title.setText("VstWindow");
  title.setFont({title.font().family(), 18});

  setStyle();
  setupGrid();
}

void VstWindow::setStyle() {
  setStyleSheet("background-color: purple;");
}

void VstWindow::setupGrid() {
  grid.setVerticalSpacing(1);

  grid.addWidget(&title, 0, 0, 1, -1);

  grid.setColumnStretch(0, 1);
  grid.setColumnStretch(1, 10);
  grid.setRowMinimumHeight(0, 20);
  grid.setRowStretch(1, 10);

  setLayout(&grid);
}

void VstWindow::show() {
  QWidget::show();
  raise();
  // Logging::write(
  //   Info,
  //   "VstWindow::show",
  //   "EditorHost called VstWindow show"
  // );
  // nativeParentWidget()->windowHandle();
 // TODO
}

void VstWindow::close() {
  // TODO
}

void VstWindow::resize(Size newSize) {
  // TODO
}

NativePlatformWindow VstWindow::getNativePlatformWindow() const {
  // #if SMTG_OS_WINDOWS
  //   const Steinberg::FIDString kPlatformString = Steinberg::kPlatformStringWin;
  // #elif SMTG_OS_IOS
  //   const Steinberg::FIDString kPlatformString = Steinberg::kPlatformStringIOS;
  // #elif SMTG_OS_MACOS
  //   const Steinberg::FIDString kPlatformString = Steinberg::kPlatformStringMac;
  // #elif SMTG_OS_LINUX
  //   const Steinberg::FIDString kPlatformString = Steinberg::kPlatformStringLinux;
  // #endif

  return {
    Steinberg::kPlatformTypeHIView,
    reinterpret_cast<void*>(winId())
  };
}

Size VstWindow::getContentSize() {
  return {
    300,
    300
  };
}

Steinberg::tresult VstWindow::queryInterface (const Steinberg::TUID iid, void** obj) {
  return 1;
};

} // Gui
} // Gj