//
// Created by ns on 5/26/25.
//

#ifndef GJDBPLUGIN_H
#define GJDBPLUGIN_H

#include <string>
#include <vector>

#include <sqlite3.h>

#include "../../Logging.h"
#include "../../../types/AtomicStr.h"
#include "../../../types/Types.h"

namespace Gj {
namespace Db {
struct Plugin {
  ID id = 0;
  PluginPath filePath;
  AtomicStr format;
  AtomicStr name;
  ChannelIndex channelIndex;
  PluginIndex pluginIndex;
  Version version;
  std::vector<uint8_t> audioHostComponentStateBlob;
  std::vector<uint8_t> audioHostControllerStateBlob;
  std::vector<uint8_t> editorHostComponentStateBlob;
  std::vector<uint8_t> editorHostControllerStateBlob;

  bool operator=(const Plugin& rhs) const {
    return id == rhs.id;
  }

  bool operator<(const Plugin& rhs) const {
    return pluginIndex < rhs.pluginIndex;
  }

  Plugin(
    ID id,
    const AtomicStr& filePath,
    const AtomicStr& format,
    const AtomicStr& name,
    ChannelIndex channelIndex,
    PluginIndex effectIndex,
    std::vector<uint8_t> audioHostComponentStateBlob,
    std::vector<uint8_t> audioHostControllerStateBlob,
    std::vector<uint8_t> editorHostComponentStateBlob,
    std::vector<uint8_t> editorHostControllerStateBlob
  );

  explicit Plugin(
    const AtomicStr& filePath,
    const AtomicStr& format,
    const AtomicStr& name,
    ChannelIndex channelIndex,
    PluginIndex pluginIdx,
    std::vector<uint8_t> audioHostComponentStateBlob,
    std::vector<uint8_t> audioHostControllerStateBlob,
    std::vector<uint8_t> editorHostComponentStateBlob,
    std::vector<uint8_t> editorHostControllerStateBlob
  );

  static Plugin deser(sqlite3_stmt* stmt);
};
} // Db
} // Gj


#endif //GJDBPLUGIN_H
