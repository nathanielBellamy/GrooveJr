//
// Created by ns on 5/28/25.
//

#ifndef SCENEREPOSITORY_H
#define SCENEREPOSITORY_H

#include <vector>
#include <optional>

#include <sqlite3.h>

#include "../../Logging.h"
#include "../../../AppState.h"

#include "../../entity/mixer/Scene.h"
#include "../../entity/mixer/Effect.h"
#include "../../entity/mixer/ChannelEntity.h"
#include "../../Types.h"

namespace Gj {
namespace Db {

class SceneRepository {
  sqlite3** db;
  AppState* gAppState;

  public:
    SceneRepository(sqlite3** db, AppState* appState);
    std::vector<Scene> getAll() const;
    ID create(const Scene& scene) const;
    std::optional<Scene> update(Scene scene) const;
    std::vector<ChannelEntity> getChannels(int sceneId) const;
    std::vector<Effect> getEffects(int sceneId) const;

    Scene findOrCreate(ID dbId) const;
    std::optional<Scene> find(ID dbId) const;

    Scene findOrCreateBySceneId(SceneID sceneId) const;
    std::optional<Scene> findBySceneId(SceneID sceneId) const;
    SceneID nextSceneId() const;
};

} // Db
} // Gj


#endif //SCENEREPOSITORY_H
