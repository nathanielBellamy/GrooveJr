//
// Created by ns on 5/26/25.
//

#ifndef EFFECTREPOSITORY_H
#define EFFECTREPOSITORY_H

#include <vector>

#include <sqlite3.h>

#include "../../Logging.h"
#include "../../../AppState.h"
#include "../../entity/mixer/Effect.h"

namespace Gj {
namespace Db {

class EffectRepository {
  sqlite3** db;
  AppState* gAppState;

  public:
    explicit EffectRepository(sqlite3** db, AppState* gAppState);
    std::vector<Effect> getAll() const;
    int save(const Effect& effect) const;
    std::vector<Effect> getBySceneId(int sceneId) const;
    std::vector<Effect> getBySceneIndex(int sceneIndex) const;
};

} // Db
} // Gj

#endif //EFFECTREPOSITORY_H
