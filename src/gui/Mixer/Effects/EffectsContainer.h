//
// Created by ns on 3/16/25.
//

#ifndef EFFECTSCONTAINER_H
#define EFFECTSCONTAINER_H

#include <iostream>
#include<memory>
#include<vector>

#include <QLabel>
#include <QWidget>
#include <QGridLayout>

#include "VstWindow.h"
#include "../../../audio/Mixer.h"

namespace Gj {
namespace Gui {

class EffectsContainer final : public QWidget {

  public:
     EffectsContainer(QWidget* parent, Audio::Mixer* mixer);

  private:
    Audio::Mixer* mixer;
    QGridLayout grid;
    QLabel title;
    void setupGrid();
    void setStyle();
};

} // Gui
} // Gj


#endif //EFFECTSCONTAINER_H
