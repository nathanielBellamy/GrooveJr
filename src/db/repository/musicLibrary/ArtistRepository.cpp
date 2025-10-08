//
// Created by ns on 10/6/25.
//

#include "ArtistRepository.h"

namespace Gj {
namespace Db {

ArtistRepository::ArtistRepository(sqlite3** db, AppState* gAppState)
  : db(db)
  , gAppState(gAppState)
  {}

ID ArtistRepository::save(const Artist& artist) const {
  const std::string query = R"sql(
    insert into artists (name)
    values (?)
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::ArtistRepository::save",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  sqlite3_bind_text(stmt, 1, artist.name.c_str(), -1, SQLITE_STATIC);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    Logging::write(
      Error,
      "Db::ArtistRepository::save",
      "Failed to save Artist " + artist.name + " Message: " + std::string(sqlite3_errmsg(*db))
    );
  } else {
    Logging::write(
      Info,
      "Db::ArtistRepository::save",
      "Saved Artist: " + artist.name
    );
  }

  return static_cast<ID>(sqlite3_last_insert_rowid(*db));
}


ID ArtistRepository::saveAll(const std::vector<Artist>& artist) const {

  // TODO

  return 0;
};

} // Db
} // Gj
