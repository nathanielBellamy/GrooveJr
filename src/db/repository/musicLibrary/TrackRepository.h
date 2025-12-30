//
// Created by ns on 5/25/25.
//

#ifndef TRACKREPOSITORY_H
#define TRACKREPOSITORY_H

#include "../../../enums/Result.h"
#include "../../entity/musicLibrary/Artist.h"
#include "../../entity/musicLibrary/Track.h"
#include "MusicLibraryRepository.h"

namespace Gj {
namespace Db {
class TrackRepository final : public MusicLibraryRepository {
public:
  TrackRepository(sqlite3** db, State::Core* stateCore)
  : MusicLibraryRepository(db, stateCore) {
  };

  std::vector<Track> getAll() const;

  ID save(const Track& track) const;

  Track findByAlbumIdAndTrackNumber(ID albumId, TrackNumber trackNumber) const;

  Result loadAll(const std::map<ID, Track>& tracks) const;

  Result join(const Track& track, const Artist& artist) const;
};
} // Db
} // Gj

#endif //TRACKREPOSITORY_H
