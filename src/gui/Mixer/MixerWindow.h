//
// Created by ns on 2/28/25.
//

#ifndef MIXERWINDOW_H
#define MIXERWINDOW_H

#include <QGridLayout>
#include <QLabel>
#include <QWidget>

#include "./Channel/DryChannel.h"
#include "./Channel/EffectsChannel/EffectsChannelsContainer.h"

namespace Gj {
namespace Gui {

class MixerWindow final : public QWidget {
  public:
    explicit MixerWindow(QWidget *parent);

  private:
    QGridLayout grid;
    QLabel title;
    DryChannel dryChannel;
    EffectsChannelsContainer effectsChannelsContainer;

    void setStyle();
    void setupGrid();
};

} // Gui
} // Gj

#endif //MIXERWINDOW_H
