//
// Created by ns on 5/28/25.
//

#ifndef SCENE_H
#define SCENE_H

#include <string>

#include <sqlite3.h>

#include "../../Types.h"
#include "../../../types/AtomicStr.h"

namespace Gj {
namespace Db {

struct Scene {
  ID id; // db primary key
  SceneID sceneId;
  AtomicStr name;
  float playbackSpeed = 1.0f;
  Version version = 0;

  Scene(ID id, SceneID sceneId, const AtomicStr& name, float playbackSpeed, int version);
  Scene(const AtomicStr& name, float playbackSpeed);
  static Scene deser(sqlite3_stmt* stmt);

  static Scene base() {
    return {
      0, 0, "Blank Scene", 1.0f, 0
    };
  }

  std::string toStdString() const {
    return std::string(" Scene { ") +
      "  dbId: " + std::to_string(id) +
      ", sceneId: " + std::to_string(sceneId) +
      ", name: " + name +
      ", playbackSpeed: " + std::to_string(playbackSpeed) + " }";
  };
};

} // Db
} // Gj


#endif //SCENE_H
