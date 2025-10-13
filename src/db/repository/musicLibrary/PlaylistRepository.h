//
// Created by ns on 10/13/25.
//

#ifndef PLAYLISTREPOSITORY_H
#define PLAYLISTREPOSITORY_H

#include <optional>

#include <sqlite3.h>

#include "../../Logging.h"
#include "../../../AppState.h"
#include "../../../enums/Result.h"

#include "../../entity/musicLibrary/Playlist.h"

namespace Gj {
namespace Db {

class PlaylistRepository final {
  sqlite3** db;
  AppState* gAppState;

  public:
    PlaylistRepository(sqlite3** db, AppState* gAppState);
    std::vector<Playlist> getAll() const;
    ID save(const Playlist& playlist) const;

    std::optional<Playlist> findByName(const std::string& name) const;
    Result addAudioFileAtTrackNumber(ID audioFileId, TrackNumber trackNumber) const;
};

} // Db
} // Gj

#endif //PLAYLISTREPOSITORY_H
