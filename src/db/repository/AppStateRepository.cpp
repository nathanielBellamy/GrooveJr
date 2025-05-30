//
// Created by ns on 5/29/25.
//

#include "AppStateRepository.h"

namespace Gj {
namespace Db {


AppStateRepository::AppStateRepository(sqlite3** db, AppState* gAppState)
  : db(db)
  , gAppState(gAppState)
  {}

} // Db
} // Gj