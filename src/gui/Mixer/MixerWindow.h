//
// Created by ns on 2/28/25.
//

#ifndef MIXERWINDOW_H
#define MIXERWINDOW_H

#include <QLabel>
#include <QWidget>

namespace Gj {
namespace Gui {

class MixerWindow final : public QWidget {
  public:
    explicit MixerWindow(QWidget *parent);

  private:
    QLabel title;
};

} // Gui
} // Gj

#endif //MIXERWINDOW_H
