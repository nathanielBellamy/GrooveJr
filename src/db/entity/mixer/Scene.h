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
  ID id;
  AtomicStr name;
  float playbackSpeed = 1.0f;
  int version = 0;

  Scene(ID id, const AtomicStr& name, float playbackSpeed, int version);
  Scene(const AtomicStr& name, float playbackSpeed);
  static Scene deser(sqlite3_stmt* stmt);

  static Scene base() {
    return {
      0, "Bank Scene", 1.0f, 0
    };
  }
};

} // Db
} // Gj


#endif //SCENE_H
