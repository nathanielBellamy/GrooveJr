//
// Created by ns on 3/8/25.
//

#ifndef MUTESOLOCONTAINER_H
#define MUTESOLOCONTAINER_H

#include <QAction>
#include <QGridLayout>
#include <QWidget>

#include "../../../../Logging.h"
#include "../MuteButton.h"
#include "../SoloButton.h"
#include "EffectsButton.h"

namespace Gj {
namespace Gui {

class MuteSoloContainer final : public QWidget {
  public:
    MuteSoloContainer(QWidget* parent, QAction* openEffectsContainer);
    ~MuteSoloContainer();

  private:
    QGridLayout grid;
    MuteButton mute;
    SoloButton solo;
    EffectsButton effects;
    void setupGrid();

};

} // Gui
} // Gj



#endif //MUTESOLOCONTAINER_H
