//
// Created by ns on 3/7/25.
//

#ifndef EFFECTSCHANNELCONTAINER_H
#define EFFECTSCHANNELCONTAINER_H

#include <memory>
#include <vector>

#include <QGridLayout>
#include <QWidget>

#include "EffectsChannel.h"

namespace Gj {
namespace Gui {

class EffectsChannelsContainer final : public QWidget {

  public:
    EffectsChannelsContainer(QWidget* parent);

  private:
    QGridLayout grid;
    QWidget spacer;
    std::vector<std::unique_ptr<EffectsChannel>> channels;
    void setStyle();
    void setupGrid();

};

} // Gui
} // Gj


#endif //EFFECTSCHANNELCONTAINER_H
