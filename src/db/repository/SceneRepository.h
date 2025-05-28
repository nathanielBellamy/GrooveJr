//
// Created by ns on 5/28/25.
//

#ifndef SCENEREPOSITORY_H
#define SCENEREPOSITORY_H

#include <vector>

#include <sqlite3.h>

#include "../../Logging.h"
#include "../../AppState.h"

#include "../entity/Scene.h"
#include "../entity/Effect.h"

namespace Gj {
namespace Db {

class SceneRepository {
  sqlite3** db;
  AppState* gAppState;

  public:
    SceneRepository(sqlite3** db, AppState* appState);
    int save(Scene scene) const;
    std::vector<Effect> getEffects(int sceneIndex) const;
};

} // Db
} // Gj


#endif //SCENEREPOSITORY_H
