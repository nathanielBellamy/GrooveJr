//
// Created by ns on 5/28/25.
//

#include "Scene.h"


namespace Gj {
namespace Db {

Scene::Scene(const int id, const int sceneIndex, const std::string& name, const int version)
  : id(id)
  , sceneIndex(sceneIndex)
  , name(name)
  , version(version)
  {}

Scene::Scene(const int sceneIndex, const std::string& name)
  : sceneIndex(sceneIndex)
  , name(name)
  {}

} // Db
} // Gj