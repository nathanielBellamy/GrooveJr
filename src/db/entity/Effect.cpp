//
// Created by ns on 5/26/25.
//

#include "Effect.h"

namespace Gj {
namespace Db {

Effect::Effect(
    const int id,
    const std::string& filePath,
    const std::string& format,
    const std::string& name,
    const int state,
    const int channelIndex,
    const int effectIndex,
    const int version
  )
  : id(id)
  , filePath(filePath)
  , format(format)
  , name(name)
  , state(state)
  , channelIndex(channelIndex)
  , effectIndex(effectIndex)
  , version(version)
  {}

Effect::Effect(
    const std::string& filePath,
    const std::string& format,
    const std::string& name,
    const int state,
    const int channelIndex,
    const int effectIndex,
    const int version
  )
  : filePath(filePath)
  , format(format)
  , name(name)
  , state(state)
  , channelIndex(channelIndex)
  , effectIndex(effectIndex)
  , version(version)
  {}

Effect Effect::deser(sqlite3_stmt *stmt) {
  const int id = sqlite3_column_int(stmt, 0);
  const unsigned char* filePath = sqlite3_column_text(stmt, 1);
  const unsigned char* format = sqlite3_column_text(stmt, 2);
  const unsigned char* name = sqlite3_column_text(stmt, 3);
  const int state = sqlite3_column_int(stmt, 4);
  const int channelIndex = sqlite3_column_int(stmt, 5);
  const int effectIndex = sqlite3_column_int(stmt, 6);
  const int version = sqlite3_column_int(stmt, 8);

  return Effect(
    id,
    std::string(reinterpret_cast<const char*>(filePath)),
    std::string(reinterpret_cast<const char*>(format)),
    std::string(reinterpret_cast<const char*>(name)),
    state,
    channelIndex,
    effectIndex,
    version
  );
}

} // Db
} // Gj