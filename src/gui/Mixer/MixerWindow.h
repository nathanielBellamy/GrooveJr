//
// Created by ns on 2/28/25.
//

#ifndef MIXERWINDOW_H
#define MIXERWINDOW_H

#include <memory>
#include <vector>

#include <QGridLayout>
#include <QLabel>
#include <QWidget>

#include "./Channel.h"

namespace Gj {
namespace Gui {

class MixerWindow final : public QWidget {
  public:
    explicit MixerWindow(QWidget *parent);

  private:
    QGridLayout grid;
    QLabel title;
    std::vector<std::unique_ptr<Channel>> channels;

    void setStyle();
    void setupGrid();
};

} // Gui
} // Gj

#endif //MIXERWINDOW_H
