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
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  title.setAlignment(Qt::AlignLeft|Qt::AlignBottom);
  title.setText("Mixer");
  title.setFont({title.font().family(), 36});
}



} // Gui
} // Gj