//
// Created by ns on 11/6/25.
//

#ifndef GJGUIHYDRATER_H
#define GJGUIHYDRATER_H

#include "QObject"

#include "../AppState.h"

namespace Gj {
namespace Gui {

class Hydrater : public QObject {
  Q_OBJECT

public:
  explicit Hydrater()
    : QObject(nullptr)
    {};
  // todo:
  // - instantiate in main window
  // - move to Display Actor Thread
  // - emit the trigger

  signals:
    void hydrate(const AppStatePacket& appStatePacket);
};

} // Gui
} // Gj

#endif //HYDRATER_H
