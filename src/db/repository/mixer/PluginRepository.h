//
// Created by ns on 5/26/25.
//

#ifndef GJDBPLUGINREPOSITORY_H
#define GJDBPLUGINREPOSITORY_H

#include <vector>

#include <sqlite3.h>

#include "../../Logging.h"
#include "../../../state/Core.h"
#include "../../entity/mixer/Plugin.h"

namespace Gj {
namespace Db {
class PluginRepository {
  sqlite3** db;
  State::Core* stateCore;

public:
  explicit PluginRepository(sqlite3** db, State::Core* stateCore);

  std::vector<Plugin> getAll() const;

  int save(const Plugin& plugin) const;

  std::vector<Plugin> getBySceneId(int sceneId) const;
};
} // Db
} // Gj

#endif //GJDBPLUGINREPOSITORY_H
