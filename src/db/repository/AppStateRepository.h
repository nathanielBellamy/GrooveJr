//
// Created by ns on 5/29/25.
//

#ifndef APPSTATEREPOSITORY_H
#define APPSTATEREPOSITORY_H

#include <sqlite3.h>

#include "../Logging.h"
#include "../../AppState.h"
#include "../entity/AppStateEntity.h"

namespace Gj {
namespace Db {

class AppStateRepository {
  sqlite3** db;
  AppState* gAppState;

  public:
    explicit AppStateRepository(sqlite3** db, AppState* gAppState);
    AppStateEntity get() const;
    int save() const;
};

} // Db
} // Gj



#endif //APPSTATEREPOSITORY_H
