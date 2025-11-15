//
// Created by ns on 5/28/25.
//

#include "Scene.h"


namespace Gj {
namespace Db {

Scene::Scene(const ID id, const AtomicStr& name, const float playbackSpeed, const int version)
  : id(id)
  , name(name)
  , playbackSpeed(playbackSpeed)
  , version(version)
  {}

Scene::Scene(const AtomicStr& name, const float playbackSpeed)
  : id(0)
  , name(name)
  , playbackSpeed(playbackSpeed)
  {}

Scene Scene::deser(sqlite3_stmt* stmt) {
  const int id = sqlite3_column_int(stmt, 0);
  const unsigned char* name = sqlite3_column_text(stmt, 1);
  const float playbackSpeed = sqlite3_column_double(stmt, 2);
  const int version = sqlite3_column_int(stmt, 3);

  return Scene(
    id,
    AtomicStr(reinterpret_cast<const char*>(name)),
    playbackSpeed,
    version
  );
}

} // Db
} // Gj