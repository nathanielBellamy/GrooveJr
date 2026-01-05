//
// Created by ns on 12/30/25.
//

#ifndef GJSTATEMIXERPLUGINPACKET_H
#define GJSTATEMIXERPLUGINPACKET_H

#include <string>

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

  PluginSlotPacket() = default;

  std::string std_str() const {
    std::string res = "\n Plugin " + std::to_string(pluginIndex) + " { \n";
    res += name.std_str() + ": " + path.std_str() + "\n";
    res += " } \\ END State::Mixer::Plugin \n";

    return res;
  }
};

template<class Inspector>
bool inspect(Inspector& f, PluginSlotPacket& x) {
  return f.object(x).fields(
    f.field("hasValue", x.hasValue),
    f.field("pluginIndex", x.pluginIndex),
    f.field("name", x.name),
    f.field("path", x.path)
  );
}
} // Mixer
} // State
} // Gj


#endif //GJSTATEMIXERPLUGINPACKET_H
