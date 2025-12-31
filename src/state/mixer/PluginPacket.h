//
// Created by ns on 12/30/25.
//

#ifndef GJSTATEMIXERPLUGINPACKET_H
#define GJSTATEMIXERPLUGINPACKET_H

#include "../../types/AtomicStr.h"
#include "../../types/Types.h"

namespace Gj {
namespace State {
namespace Mixer {
struct PluginSlotPacket {
  bool hasValue = false;
  PluginIndex pluginIndex = 0;
  AtomicStr name = AtomicStr(" - ");
  PluginPath path = AtomicStr(" - ");
};
} // Mixer
} // State
} // Gj


#endif //GJSTATEMIXERPLUGINPACKET_H
