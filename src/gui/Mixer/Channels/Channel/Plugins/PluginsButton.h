//
// Created by ns on 3/25/25.
//

#ifndef GJGUIMIXERPLUGINSBUTTON_H
#define GJGUIMIXERPLUGINSBUTTON_H

#include <QAction>
#include <QPushButton>

#include "../../../../Color.h"

namespace Gj {
namespace Gui {
namespace Mixer {
class PluginsButton final : public QPushButton {
public:
  PluginsButton(QWidget* parent, QAction* openPluginsContainer);

private:
  QAction* openPluginsContainer;

  void setStyle();

  void mousePressEvent(QMouseEvent* event) override;
};
} // Mixer
} // Gui
} // Gj

#endif //GJGUIMIXERPLUGINSBUTTON_H
