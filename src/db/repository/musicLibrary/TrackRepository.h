//
// Created by ns on 5/25/25.
//

#ifndef TRACKREPOSITORY_H
#define TRACKREPOSITORY_H

#include <sqlite3.h>

#include "../../Logging.h"
#include "../../../AppState.h"

#include "../../entity/musicLibrary/Track.h"

namespace Gj {
namespace Db {

class TrackRepository {
  sqlite3** db;
  AppState* gAppState;

  public:
    TrackRepository(sqlite3** db, AppState* gAppState);
    std::vector<Track> getAll() const;

};

} // Db
} // Gj

#endif //TRACKREPOSITORY_H
