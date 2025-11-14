//
// Created by ns on 5/26/25.
//

#ifndef EFFECT_H
#define EFFECT_H

#include <string>
#include <vector>

#include <sqlite3.h>

#include "../../Logging.h"
#include "../../../types/AtomicStr.h"

namespace Gj {
namespace Db {

struct Effect {
  int id = 0;
  AtomicStr filePath;
  AtomicStr format;
  AtomicStr name;
  int channelIndex;
  int effectIndex;
  int version;
  std::vector<uint8_t> audioHostComponentStateBlob;
  std::vector<uint8_t> audioHostControllerStateBlob;
  std::vector<uint8_t> editorHostComponentStateBlob;
  std::vector<uint8_t> editorHostControllerStateBlob;

  bool operator=(const Effect& rhs) const {
    return id == rhs.id;
  }
  bool operator<(const Effect& rhs) const {
    return effectIndex < rhs.effectIndex;
  }

  Effect(
    int id,
    const AtomicStr& filePath,
    const AtomicStr& format,
    const AtomicStr& name,
    int channelIndex,
    int effectIndex,
    std::vector<uint8_t> audioHostComponentStateBlob,
    std::vector<uint8_t> audioHostControllerStateBlob,
    std::vector<uint8_t> editorHostComponentStateBlob,
    std::vector<uint8_t> editorHostControllerStateBlob
  );

  explicit Effect(
    const AtomicStr& filePath,
    const AtomicStr& format,
    const AtomicStr& name,
    int channelIndex,
    int effectIndex,
    std::vector<uint8_t> audioHostComponentStateBlob,
    std::vector<uint8_t> audioHostControllerStateBlob,
    std::vector<uint8_t> editorHostComponentStateBlob,
    std::vector<uint8_t> editorHostControllerStateBlob
  );
  static Effect deser(sqlite3_stmt* stmt);
};

} // Db
} // Gj


#endif //EFFECT_H
