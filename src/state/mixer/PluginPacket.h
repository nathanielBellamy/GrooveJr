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

struct PluginPacket {
  AtomicStr name;
  PluginPath path;
};

} // Mixer
} // State
} // Gj


#endif //GJSTATEMIXERPLUGINPACKET_H
