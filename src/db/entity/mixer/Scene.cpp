//
// Created by ns on 5/28/25.
//

#include "Scene.h"


namespace Gj {
namespace Db {
Scene::Scene(
  const ID id,
  const SceneID sceneId,
  const AtomicStr& name,
  const float playbackSpeed,
  const sf_count_t crossfade,
  const int version)
: id(id)
  , sceneId(sceneId)
  , name(name)
  , playbackSpeed(playbackSpeed)
  , crossfade(crossfade)
  , version(version) {
}

Scene::Scene(const AtomicStr& name, const float playbackSpeed, sf_count_t crossfade)
: id(0)
  , sceneId(0)
  , name(name)
  , playbackSpeed(playbackSpeed)
  , crossfade(crossfade) {
}

Scene Scene::deser(sqlite3_stmt* stmt) {
  const ID id = sqlite3_column_int(stmt, 0);
  const SceneID sceneId = sqlite3_column_int(stmt, 1);
  const unsigned char* name = sqlite3_column_text(stmt, 2);
  const float playbackSpeed = sqlite3_column_double(stmt, 3);
  const sf_count_t crossfade = sqlite3_column_int(stmt, 4);
  const int version = sqlite3_column_int(stmt, 5);

  return {
    id,
    sceneId,
    AtomicStr(reinterpret_cast<const char*>(name)),
    playbackSpeed,
    crossfade,
    version
  };
}
} // Db
} // Gj
