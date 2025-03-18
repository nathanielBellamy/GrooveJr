//
// Created by ns on 3/16/25.
//

#ifndef EFFECTSCONTAINER_H
#define EFFECTSCONTAINER_H

#include<memory>
#include<vector>

#include <QLabel>
#include <QWidget>
#include <QGridLayout>

#include "VstWindow.h"

namespace Gj {
namespace Gui {

class EffectsContainer final : public QWidget {

  public:
     EffectsContainer(QWidget* parent);

  private:
    QGridLayout grid;
    QLabel title;
    std::vector<std::unique_ptr<VstWindow>> vstWindows;
    void setupGrid();
    void setStyle();
};

} // Gui
} // Gj


#endif //EFFECTSCONTAINER_H
