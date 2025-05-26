//
// Created by ns on 5/25/25.
//

#ifndef TRACKREPOSITORY_H
#define TRACKREPOSITORY_H

#include <sqlite3.h>

#include "../../Logging.h"

#include "../entity/Track.h"

namespace Gj {
namespace Db {

class TrackRepository {
  sqlite3** db;

  public:
    TrackRepository(sqlite3** db);
    std::vector<Track> getAll() const;

};

} // Db
} // Gj

#endif //TRACKREPOSITORY_H
