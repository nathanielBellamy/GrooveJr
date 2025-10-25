//
// Created by ns on 10/6/25.
//

#include "ArtistRepository.h"

namespace Gj {
namespace Db {

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
    const char* errmsg = sqlite3_errmsg(*db);

    if (std::strstr(errmsg, "UNIQUE constraint failed: artists.name") != nullptr) {
      const Artist found = findByName(artist.name);
      Logging::write(
          Warning,
          "Db::ArtistRepository::save",
          "Not Saving Artist " + artist.name + " ArtistId " + std::to_string(found.id) + ". Already Exists - SQLite UNIQUE constraint failed: artists.name"
      );
      return found.id;
    }

    Logging::write(
      Error,
      "Db::ArtistRepository::save",
      "Failed to save Artist " + artist.name + " Message: " + std::string(errmsg)
    );
    return 0;
  }

  Logging::write(
    Info,
    "Db::ArtistRepository::save",
    "Saved Artist: " + artist.name
  );
  return static_cast<ID>(sqlite3_last_insert_rowid(*db));
}

Artist ArtistRepository::findByName(const std::string& name) const {
  const std::string query = R"sql(
    select * from artists a
    where a.name = ?;
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::ArtistRepository::findByName",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return Artist::unknown();
  }

  sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

  if (sqlite3_step(stmt) == SQLITE_ROW)
    return Artist::deser(stmt);

  return Artist::unknown();
}


ID ArtistRepository::saveAll(const std::vector<Artist>& artist) const {

  // TODO

  return 0;
};

} // Db
} // Gj
