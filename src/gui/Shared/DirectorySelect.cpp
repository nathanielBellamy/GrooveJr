//
// Created by ns on 10/10/25.
//

#include "DirectorySelect.h"

namespace Gj {
namespace Gui {

DirectorySelect::~DirectorySelect() {
  Logging::write(
    Info,
    "Gui::DirectorySelect::~DirectorySelect",
    "Destroying DirectorySelect"
  );
}

DirectorySelect::DirectorySelect(QWidget* parent)
  : QFileDialog(parent)
  // TODO: default URL by system
  , defaultUrl(QUrl("/Users/ns/Music"))
  {

  setFileMode(QFileDialog::ExistingFile);
  setNameFilter(tr("AUDIO Files (*.mp3) (*.mp4) (*.wav) (*.aiff) (*.ogg) (*.alac) (*.flac)"));
  setDirectoryUrl(defaultUrl);
}

} // Gui
} // Gj
