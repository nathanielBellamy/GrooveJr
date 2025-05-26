//
// Created by ns on 5/25/25.
//

#include "TrackRepository.h"

namespace Gj {
namespace Db {

TrackRepository::TrackRepository(sqlite3** db)
  : db(db)
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
      "TrackRepository::getAll",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return tracks;
  }

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const auto track = Track::deser(stmt);
    std::cout << "Row: ID = " << track.id << ", filePath = " << track.filePath << ", title = " << track.title << std::endl;
    tracks.push_back(track);
  }

  return tracks;
}

} // Db
} // Gj
