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
  float playbackSpeed = 1.0f;
  int version = 0;

  Scene(int id, int sceneIndex, const std::string& name, float playbackSpeed, int version);
  Scene(int sceneIndex, const std::string& name, float playbackSpeed);
  static Scene deser(sqlite3_stmt* stmt);
};

} // Db
} // Gj


#endif //SCENE_H
