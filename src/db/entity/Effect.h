//
// Created by ns on 5/26/25.
//

#ifndef EFFECT_H
#define EFFECT_H

#include <string>

#include <sqlite3.h>

namespace Gj {
namespace Db {

struct Effect {

  int id = 0;
  std::string filePath;
  std::string format;
  std::string name;
  int state; // TODO
  int channelIndex;
  int effectIndex;
  int version;
  bool operator=(const Effect& rhs) const {
    return id == rhs.id;
  }
  bool operator<(const Effect& rhs) const {
    return effectIndex < rhs.effectIndex;
  }

  Effect(
    int id,
    const std::string& filePath,
    const std::string& format,
    const std::string& name,
    int state,
    int channelIndex,
    int effectIndex,
    int version
  );

  explicit Effect(
    const std::string& filePath,
    const std::string& format,
    const std::string& name,
    int state,
    int channelIndex,
    int effectIndex,
    int version
  );
  static Effect deser(sqlite3_stmt* stmt);
};

} // Db
} // Gj


#endif //EFFECT_H
