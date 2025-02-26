//
// Created by ns on 2/26/25.
//

#include "FolderSelect.h"

namespace Gj {
namespace Gui {

FolderSelect::FolderSelect(QWidget* parent)
  : QFileDialog(parent)
  {

  setFileMode(QFileDialog::Directory);
}

FolderSelect::~FolderSelect() {
  delete this;
}

} // Gui
} // Gj