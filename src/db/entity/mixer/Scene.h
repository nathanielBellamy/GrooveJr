//
// Created by ns on 5/28/25.
//

#ifndef SCENE_H
#define SCENE_H

#include <sstream>
#include <string>

#include <sqlite3.h>
#include <sndfile.h>

#include "../../../types/Types.h"
#include "../../../types/AtomicStr.h"

namespace Gj {
namespace Db {
struct Scene {
  ID id; // db primary key
  SceneID sceneId;
  AtomicStr name;
  float playbackSpeed = 1.0f;
  sf_count_t crossfade = 0LL;
  Version version = 0;

  Scene(ID id, SceneID sceneId, const AtomicStr& name, float playbackSpeed, sf_count_t crossfade, int version);

  Scene(const AtomicStr& name, float playbackSpeed, sf_count_t crossfade);

  static Scene deser(sqlite3_stmt* stmt);

  static Scene base() {
    return {
      0, 0, "Blank Scene", 1.0f, 0LL, 0
    };
  }

  std::string toString() const {
    std::ostringstream stream;
    stream
        << "Scene { "
        << "dbId: " << id
        << ", sceneId: " << sceneId
        << ", name: " << name.std_str()
        << ", playbackSpeed: " << playbackSpeed
        << ", version: " << version
        << " }";
    return stream.str();
  }

  std::string toStdString() const {
    return toString();
  };
};
} // Db
} // Gj


#endif //SCENE_H
