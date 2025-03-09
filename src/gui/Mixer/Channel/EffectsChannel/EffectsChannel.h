//
// Created by ns on 3/7/25.
//

#ifndef GUIEFFECTSCHANNEL_H
#define GUIEFFECTSCHANNEL_H

#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QWidget>

#include "EffectsSlots.h"
#include "MuteSoloContainer.h"

namespace Gj {
namespace Gui {

class EffectsChannel final : public QWidget {

  public:
    EffectsChannel(QWidget* parent, int channelIndex);

  private:
    int channelIndex;
    QGridLayout grid;
    QLabel title;
    QSlider slider;
    EffectsSlots effectsSlots;
    MuteSoloContainer muteSoloContainer;
    void setStyle();
    void setupGrid();
};


} // Gui
} // Gj

#endif //EFFECTSCHANNEL_H
