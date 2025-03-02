//
// Created by ns on 3/2/25.
//

#include "MusicLibraryWindow.h"

namespace Gj {
namespace Gui {

MusicLibraryWindow::MusicLibraryWindow(QWidget* parent)
  : QWidget(parent)
  , title(this)
  {
  setStyle();
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  title.setAlignment(Qt::AlignLeft|Qt::AlignBottom);
  title.setText("Music Library");
  title.setFont({title.font().family(), 36});
}

void MusicLibraryWindow::setStyle() {
  setStyleSheet("background-color: cyan;");
}

} // Gui
} // Gj
