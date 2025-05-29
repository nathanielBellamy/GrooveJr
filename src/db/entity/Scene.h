//
// Created by ns on 5/28/25.
//

#ifndef SCENE_H
#define SCENE_H

#include <string>

#include <sqlite3.h>

namespace Gj {
namespace Db {

struct Scene {
  int id;
  int sceneIndex;
  std::string name;
  int version = 0;

  Scene(int id, int sceneIndex, const std::string& name, int version);
  Scene(int sceneIndex, const std::string& name);
  static Scene deser(sqlite3_stmt* stmt);
};

} // Db
} // Gj


#endif //SCENE_H
