//
// Created by ns on 5/28/25.
//

#include "Scene.h"


namespace Gj {
namespace Db {

Scene::Scene(const int id, const int index, const std::string& name, const int version)
  : id(id)
  , index(index)
  , name(name)
  , version(version)
  {}

Scene::Scene(const int index, const std::string& name)
  : index(index)
  , name(name)
  {}

} // Db
} // Gj