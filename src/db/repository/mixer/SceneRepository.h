//
// Created by ns on 5/28/25.
//

#ifndef SCENEREPOSITORY_H
#define SCENEREPOSITORY_H

#include <vector>
#include <optional>

#include <sqlite3.h>

#include "../../Logging.h"
#include "../../../state/AppState.h"

#include "../../entity/mixer/Scene.h"
#include "../../entity/mixer/Plugin.h"
#include "../../entity/mixer/ChannelEntity.h"
#include "../../../types/Types.h"

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

  std::vector<ChannelEntity> getChannels(ID sceneId) const;

  std::vector<Plugin> getPlugins(ID sceneId) const;

  Scene findOrCreate(ID dbId) const;

  std::optional<Scene> find(ID dbId) const;

  Scene findOrCreateBySceneId(SceneID sceneId) const;

  std::optional<Scene> findBySceneId(SceneID sceneId) const;

  std::optional<SceneID> nextSceneId() const;

  int sceneCount() const;
};
} // Db
} // Gj


#endif //SCENEREPOSITORY_H
