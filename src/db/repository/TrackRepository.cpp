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
  std::string query = R"sql(
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
    int id = sqlite3_column_int(stmt, 0);
    const unsigned char* filePath = sqlite3_column_text(stmt, 1);
    const unsigned char* title = sqlite3_column_text(stmt, 2);

    std::cout << "Row: ID = " << id << ", filePath = " << (filePath ? reinterpret_cast<const char*>(filePath) : "NULL") << '\n';
  }

  return tracks;
}

} // Db
} // Gj
