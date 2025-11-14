//
// Created by ns on 5/28/25.
//

#include "Scene.h"


namespace Gj {
namespace Db {

Scene::Scene(const int id, const int sceneIndex, const std::string& name, const float playbackSpeed, const int version)
  : id(id)
  , sceneIndex(sceneIndex)
  , name(name)
  , playbackSpeed(playbackSpeed)
  , version(version)
  {}

Scene::Scene(const int sceneIndex, const std::string& name, const float playbackSpeed)
  : sceneIndex(sceneIndex)
  , name(name)
  , playbackSpeed(playbackSpeed)
  {}

Scene Scene::deser(sqlite3_stmt* stmt) {
  const int id = sqlite3_column_int(stmt, 0);
  const int sceneIndex = sqlite3_column_int(stmt, 1);
  const unsigned char* name = sqlite3_column_text(stmt, 2);
  const float playbackSpeed = sqlite3_column_double(stmt, 3);
  const int version = sqlite3_column_int(stmt, 4);

  return Scene(
    id,
    sceneIndex,
    std::string(reinterpret_cast<const char*>(name)),
    playbackSpeed,
    version
  );
}

} // Db
} // Gj