//
// Created by ns on 5/28/25.
//

#include "Scene.h"


namespace Gj {
namespace Db {

Scene::Scene(const ID id, const SceneID sceneId, const AtomicStr& name, const float playbackSpeed, const int version)
  : id(id)
  , sceneId(sceneId)
  , name(name)
  , playbackSpeed(playbackSpeed)
  , version(version)
  {}

Scene::Scene(const AtomicStr& name, const float playbackSpeed)
  : id(0)
  , sceneId(0)
  , name(name)
  , playbackSpeed(playbackSpeed)
  {}

Scene Scene::deser(sqlite3_stmt* stmt) {
  const ID id = sqlite3_column_int(stmt, 0);
  const SceneID sceneId = sqlite3_column_int(stmt, 1);
  const unsigned char* name = sqlite3_column_text(stmt, 2);
  const float playbackSpeed = sqlite3_column_double(stmt, 3);
  const int version = sqlite3_column_int(stmt, 4);

  return {
    id,
    sceneId,
    AtomicStr(reinterpret_cast<const char*>(name)),
    playbackSpeed,
    version
  };
}

} // Db
} // Gj