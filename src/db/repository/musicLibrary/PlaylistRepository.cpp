//
// Created by ns on 10/13/25.
//

#include "PlaylistRepository.h"

namespace Gj {
namespace Db {

std::vector<Playlist> PlaylistRepository::getAll() const {
  std::vector<Playlist> playlists;
  const std::string query = R"sql(
    select * from playlists;
  )sql";

  sqlite3_stmt* stmt;

  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::PlaylistRepository::getAll",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return playlists;
  }

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const auto playlist = Playlist::deser(stmt);
    playlists.push_back(playlist);
  }

  return playlists;
}

ID PlaylistRepository::save(const Playlist& playlist) const {
  const std::string query = R"sql(
    insert into playlists (name)
    values (?)
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::PlaylistRepository::save",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  sqlite3_bind_text(stmt, 1, playlist.name.c_str(), -1, SQLITE_STATIC);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    const char* errmsg = sqlite3_errmsg(*db);
    if (std::strstr(errmsg, "UNIQUE constraint failed: playlists.name") != nullptr) {
      const std::optional<Playlist> found = findByName(playlist.name);
      if (!found.has_value()) {
        Logging::write(
          Error,
          "Db::PlaylistRepository::save(playlist)",
          "Unable to find duplicate playlist named: " + std::string(playlist.name) + ". This Error should not happen."
        );
        return 0;
      }
      Logging::write(
          Warning,
          "Db::ArtistRepository::save",
          "Not Saving Playlist " + playlist.name + " as a Playlist with that name already exists - SQLite UNIQUE constraint failed: playlists.name"
      );
      return found.value().id;
    }
    Logging::write(
      Error,
      "Db::PlaylistRepository::save",
      "Failed to save Playlist " + playlist.name + " Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  Logging::write(
    Info,
    "Db::PlaylistRepository::save",
    "Saved Playlist: " + playlist.name
  );
  return static_cast<ID>(sqlite3_last_insert_rowid(*db));
}

std::optional<Playlist> PlaylistRepository::findByName(const std::string& name) const {
  const std::string query = R"sql(
    select * from playlists p
    where p.name = ?
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::PlaylistRepository::findByName",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return std::optional<Playlist>();
  }

  sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

  if (sqlite3_step(stmt) == SQLITE_ROW)
    return std::optional(Playlist::deser(stmt));

  return std::optional<Playlist>();
}

Result PlaylistRepository::addAudioFileAtTrackNumber(ID audioFileId, TrackNumber trackNumber) const {

  // TODO
  // - update all subsequent track numbers for the tracks in the playlist

  return OK;
}

} // Db
} // Gj
