//
// Created by ns on 3/16/25.
//

#include "VstWindow.h"

#include "../../../../../types/Types.h"

namespace Gj {
namespace Gui {

VstWindow::VstWindow(QWidget* parent, ChannelIndex channelIndex, EffectIndex effectIndex, const AtomicStr& name)
  : QWidget(parent)
  , channelIndex(channelIndex)
  , effectIndex(effectIndex)
  , name(name)
  , grid(this)
  {

  setWindowTitle("Channel " + QString::number(channelIndex) + " - Effect " + QString::number(effectIndex + 1) + " - " + name.c_str());

  setStyle();
  setupGrid();
}

VstWindow::~VstWindow() noexcept {
  Logging::write(
    Info,
    "Gui::VstWindow::~VstWindow",
    "Destroying VstWindow"
  );
}


void VstWindow::setStyle() {
  setMinimumSize(QSize(800, 400));
}

void VstWindow::setupGrid() {
  grid.setVerticalSpacing(1);

  grid.setColumnStretch(0, 1);
  grid.setColumnStretch(1, 10);
  grid.setRowMinimumHeight(0, 20);
  grid.setRowStretch(1, 10);

  setLayout(&grid);
}

void VstWindow::show() {
  QWidget::show();
  activateWindow();
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
  QWidget::close();
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
    600,
    600
  };
}

Steinberg::tresult VstWindow::queryInterface (const Steinberg::TUID iid, void** obj) {
  return 1;
};

} // Gui
} // Gj