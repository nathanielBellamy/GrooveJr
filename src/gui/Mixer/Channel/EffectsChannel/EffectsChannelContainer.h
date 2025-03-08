//
// Created by ns on 3/7/25.
//

#ifndef EFFECTSCHANNELCONTAINER_H
#define EFFECTSCHANNELCONTAINER_H

#include <QGridLayout>
#include <QWidget>

#include "EffectsChannel.h"

namespace Gj {
namespace Gui {

class EffectsChannelContainer final : public QWidget {

  public:
    EffectsChannelContainer(QWidget* parent);

  private:
    QGridLayout grid;
    std::vector<std::unique_ptr<EffectsChannel>> channels;
    void setStyle();
    void setupGrid();

};

} // Gui
} // Gj


#endif //EFFECTSCHANNELCONTAINER_H
