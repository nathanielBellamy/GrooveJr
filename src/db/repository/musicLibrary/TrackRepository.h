//
// Created by ns on 5/25/25.
//

#ifndef TRACKREPOSITORY_H
#define TRACKREPOSITORY_H

#include <sqlite3.h>

#include "../../Logging.h"
#include "../../../AppState.h"
#include "../../../enums/Result.h"

#include "../../entity/musicLibrary/Track.h"

namespace Gj {
namespace Db {

class TrackRepository final {
  sqlite3** db;
  AppState* gAppState;

  public:
    TrackRepository(sqlite3** db, AppState* gAppState);
    std::vector<Track> getAll() const;
    ID save(const Track& track) const;
    Track findByAlbumIdAndTrackNumber(ID albumId, TrackNumber trackNumber) const;
    Result loadAll(const std::map<ID, Track>& tracks) const;

};

} // Db
} // Gj

#endif //TRACKREPOSITORY_H
