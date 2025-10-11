//
// Created by ns on 5/25/25.
//

#include "TrackRepository.h"

namespace Gj {
namespace Db {

TrackRepository::TrackRepository(sqlite3** db, AppState* gAppState)
  : db(db)
  , gAppState(gAppState)
  {}

std::vector<Track> TrackRepository::getAll() const {
  std::vector<Track> tracks;
  const std::string query = R"sql(
    select * from tracks;
  )sql";

  sqlite3_stmt* stmt;

  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::TrackRepository::getAll",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return tracks;
  }

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const auto track = Track::deser(stmt);
    tracks.push_back(track);
  }

  return tracks;
}

ID TrackRepository::save(const Track& track) const {
  const std::string query = R"sql(
    insert into tracks (albumId, title, trackNumber)
    values (?, ?, ?)
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::TrackRepository::save",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  sqlite3_bind_int(stmt, 1, track.albumId);
  sqlite3_bind_text(stmt, 2, track.title.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 3, track.trackNumber);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    const char* errmsg = sqlite3_errmsg(*db);

    if (std::strstr(errmsg, "UNIQUE constraint failed: tracks.albumId, tracks.trackNumber") != nullptr) {
      const Track found = findByAlbumIdAndTrackNumber(track.albumId, track.trackNumber);
      Logging::write(
          Warning,
          "Db::ArtistRepository::save",
          "Not Saving Track " + track.title + " as TrackNumber " + std::to_string(found.trackNumber) + " already exists for AlbumId " + std::to_string(found.albumId) + " with title " + found.title + " - SQLite UNIQUE constraint failed: tracks.albumId, tracks.trackNumber"
      );
      return found.id;
    }
    Logging::write(
      Error,
      "Db::TrackRepository::save",
      "Failed to save Track " + track.title + " Message: " + std::string(sqlite3_errmsg(*db))
    );
  } else {
    Logging::write(
      Info,
      "Db::TrackRepository::save",
      "Saved Track: " + track.title
    );
  }

  return static_cast<ID>(sqlite3_last_insert_rowid(*db));
}

Track TrackRepository::findByAlbumIdAndTrackNumber(const ID albumId, const TrackNumber trackNumber) const {
  const std::string query = R"sql(
    select * from tracks t
    where t.albumId = ?
      and t.trackNumber = ?;
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::TrackRepository::getEffects",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return Track::empty();
  }

  sqlite3_bind_int(stmt, 1, albumId);
  sqlite3_bind_int(stmt, 2, trackNumber);

  if (sqlite3_step(stmt) == SQLITE_ROW)
    return Track::deser(stmt);

  return Track::empty();
}

Result TrackRepository::loadAll(const std::map<ID, Track>& tracks) const {
  // TODO
  // - fetch all tracks
  // - populate into map
  return OK;
}

} // Db
} // Gj
