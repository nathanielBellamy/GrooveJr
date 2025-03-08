//
// Created by ns on 3/8/25.
//

#ifndef MAINDRYCONTAINER_H
#define MAINDRYCONTAINER_H

#include <QGridLayout>
#include <QWidget>

#include "DryChannel.h"
#include "EffectsChannel/EffectsChannel.h"

namespace Gj {
namespace Gui {

class MainDryContainer final : public QWidget {

  public:
    MainDryContainer(QWidget* parent);

  private:
    QGridLayout grid;
    DryChannel dryChannel;
    EffectsChannel mainChannel;
    void setStyle();
    void setupGrid();
};

} // Gui
} // Gj


#endif //MAINDRYCONTAINER_H
