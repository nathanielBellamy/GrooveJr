//
// Created by ns on 5/26/25.
//

#ifndef EFFECTREPOSITORY_H
#define EFFECTREPOSITORY_H

#include <vector>

#include <sqlite3.h>

#include "../../Logging.h"
#include "../entity/Effect.h"

namespace Gj {
namespace Db {

class EffectRepository {
  sqlite3** db;

  public:
    explicit EffectRepository(sqlite3** db);
    std::vector<Effect> getAll() const;
    int save(Effect effect) const;
};

} // Db
} // Gj

#endif //EFFECTREPOSITORY_H
