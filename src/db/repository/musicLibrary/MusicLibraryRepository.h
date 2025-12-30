//
// Created by ns on 10/19/25.
//

#ifndef MUSICLIBRARYREPOSITORY_H
#define MUSICLIBRARYREPOSITORY_H

#include <sqlite3.h>

#include "../../Logging.h"
#include "../../../state/AppState.h"

namespace Gj {
namespace Db {
class MusicLibraryRepository {
protected:
  sqlite3** db;
  AppState* gAppState;

public:
  MusicLibraryRepository(sqlite3** db, AppState* gAppState)
  : db(db)
    , gAppState(gAppState) {
  };
};
} // Db
} // Gj


#endif //MUSICLIBRARYREPOSITORY_H
