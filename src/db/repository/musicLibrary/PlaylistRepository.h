//
// Created by ns on 10/13/25.
//

#ifndef PLAYLISTREPOSITORY_H
#define PLAYLISTREPOSITORY_H

#include <optional>

#include "MusicLibraryRepository.h"
#include "../../../enums/Result.h"
#include "../../entity/musicLibrary/Playlist.h"

namespace Gj {
namespace Db {

class PlaylistRepository final : public MusicLibraryRepository {

  public:
    PlaylistRepository(sqlite3** db, State::Core* stateCore)
      : MusicLibraryRepository(db, stateCore)
      {};

    std::vector<Playlist> getAll() const;
    ID save(const Playlist& playlist) const;

    std::optional<Playlist> findByName(const AtomicStr& name) const;
    Result addAudioFileAtTrackNumber(ID audioFileId, TrackNumber trackNumber) const;
};

} // Db
} // Gj

#endif //PLAYLISTREPOSITORY_H
