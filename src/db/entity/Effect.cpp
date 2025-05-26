//
// Created by ns on 5/26/25.
//

#include "Effect.h"

namespace Gj {
namespace Db {

Effect::Effect(
    const int id,
    const std::string filePath,
    const std::string name,
    const int state,
    const int channelIndex,
    const int effectIndex
  )
  : id(id)
  , filePath(filePath)
  , name(name)
  , state(state)
  , channelIndex(channelIndex)
  , effectIndex(effectIndex)
  {}

Effect Effect::deser(sqlite3_stmt *stmt) {
  const int id = sqlite3_column_int(stmt, 0);
  const unsigned char* filePath = sqlite3_column_text(stmt, 1);
  const unsigned char* name = sqlite3_column_text(stmt, 2);
  const int state = sqlite3_column_int(stmt, 3);
  const int channelIndex = sqlite3_column_int(stmt, 4);
  const int effectIndex = sqlite3_column_int(stmt, 5);

  return Effect(
    id,
    std::string(reinterpret_cast<const char*>(filePath)),
    std::string(reinterpret_cast<const char*>(name)),
    state,
    channelIndex,
    effectIndex
  );
}

} // Db
} // Gj