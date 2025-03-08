//
// Created by ns on 3/2/25.
//

#ifndef DRYCHANNEL_H
#define DRYCHANNEL_H

#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QWidget>

#include "MuteButton.h"
#include "SoloButton.h"

namespace Gj {
namespace Gui {

class DryChannel final : public QWidget {

  public:
    DryChannel(QWidget* parent);

  private:
    QGridLayout grid;
    QLabel title;
    QSlider slider;
    MuteButton mute;
    SoloButton solo;
    void setStyle();
    void setupGrid();
};

} // Gui
} // Gj



#endif //DRYCHANNEL_H
