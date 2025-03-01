//
// Created by ns on 2/28/25.
//

#include "MixerWindow.h"

namespace Gj {
namespace Gui {

MixerWindow::MixerWindow(QWidget* parent)
  : QWidget(parent)
  , title(this)
  {
  title.setAlignment(Qt::AlignLeft|Qt::AlignBottom);
  title.setText("Mixer");
}



} // Gui
} // Gj