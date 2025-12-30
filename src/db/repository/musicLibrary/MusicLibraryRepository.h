//
// Created by ns on 10/19/25.
//

#ifndef MUSICLIBRARYREPOSITORY_H
#define MUSICLIBRARYREPOSITORY_H

#include <sqlite3.h>

#include "../../Logging.h"
#include "../../../state/Core.h"

namespace Gj {
namespace Db {
class MusicLibraryRepository {
protected:
  sqlite3** db;
  State::Core* stateCore;

public:
  MusicLibraryRepository(sqlite3** db, State::Core* stateCore)
  : db(db)
    , stateCore(stateCore) {
  };
};
} // Db
} // Gj


#endif //MUSICLIBRARYREPOSITORY_H
