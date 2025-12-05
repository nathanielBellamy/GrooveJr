//
// Created by ns on 5/26/25.
//

#ifndef EFFECTREPOSITORY_H
#define EFFECTREPOSITORY_H

#include <vector>

#include <sqlite3.h>

#include "../../Logging.h"
#include "../../../AppState.h"
#include "../../entity/mixer/Plugin.h"

namespace Gj {
namespace Db {

class PluginRepository {
  sqlite3** db;
  AppState* gAppState;

  public:
    explicit PluginRepository(sqlite3** db, AppState* gAppState);
    std::vector<Plugin> getAll() const;
    int save(const Plugin& plugin) const;
    std::vector<Plugin> getBySceneId(int sceneId) const;
};

} // Db
} // Gj

#endif //EFFECTREPOSITORY_H
