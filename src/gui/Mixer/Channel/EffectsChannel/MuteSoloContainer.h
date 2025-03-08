//
// Created by ns on 3/8/25.
//

#ifndef MUTESOLOCONTAINER_H
#define MUTESOLOCONTAINER_H

#include <QGridLayout>
#include <QWidget>

#include "../MuteButton.h"
#include "../SoloButton.h"

namespace Gj {
namespace Gui {

class MuteSoloContainer final : public QWidget {
  public:
    MuteSoloContainer(QWidget* parent);

  private:
    QGridLayout grid;
    MuteButton mute;
    SoloButton solo;
    void setupGrid();

};

} // Gui
} // Gj



#endif //MUTESOLOCONTAINER_H
