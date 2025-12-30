//
// Created by ns on 11/6/25.
//

#ifndef GJGUIHYDRATER_H
#define GJGUIHYDRATER_H

#include "QObject"

#include "../state/Core.h"

namespace Gj {
namespace Gui {
class Hydrater : public QObject {
  Q_OBJECT

public:
  explicit Hydrater()
  : QObject(nullptr) {
  };

signals:
  void hydrate(const State::Packet& statePacket);
};
} // Gui
} // Gj

#endif //HYDRATER_H
