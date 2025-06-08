//
// Created by ns on 5/28/25.
//

#ifndef SCENEREPOSITORY_H
#define SCENEREPOSITORY_H

#include <vector>

#include <sqlite3.h>

#include "../Logging.h"
#include "../../AppState.h"

#include "../entity/Scene.h"
#include "../entity/Effect.h"
#include "../entity/ChannelEntity.h"

namespace Gj {
namespace Db {

class SceneRepository {
  sqlite3** db;
  AppState* gAppState;

  public:
    SceneRepository(sqlite3** db, AppState* appState);
    std::vector<Scene> getAll() const;
    int save(const Scene& scene) const;
    std::vector<ChannelEntity> getChannels(int sceneId) const;
    std::vector<Effect> getEffects(int sceneId) const;
    int findOrCreateBySceneIndex(int sceneIndex) const;
};

} // Db
} // Gj


#endif //SCENEREPOSITORY_H
