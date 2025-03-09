//
// Created by ns on 2/27/25.
//

#include "VstSelect.h"


namespace Gj {
namespace Gui {

VstSelect::~VstSelect() {
  delete this;
}

VstSelect::VstSelect(QWidget* parent)
  : QFileDialog(parent)
  // TODO: default URL by system
  , defaultUrl(QUrl("file:///Library/Audio/Plug-Ins/VST3"))
  {

  setFileMode(QFileDialog::ExistingFile);
  setNameFilter(tr("VST3 Files (*.vst3)"));
  setDirectoryUrl(defaultUrl);
}

} // Gui
} // Gj