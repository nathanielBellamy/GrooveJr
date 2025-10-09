//
// Created by ns on 10/6/25.
//

#include "GenreRepository.h"

namespace Gj {
namespace Db {

ID GenreRepository::save(const Genre& genre) const {
  const std::string query = R"sql(
    insert into genres (name)
    values (?)
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::GenreRepository::save",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  sqlite3_bind_text(stmt, 1, genre.name.c_str(), -1, SQLITE_STATIC);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    const char* errmsg = sqlite3_errmsg(*db);

    if (std::strstr(errmsg, "UNIQUE constraint failed: genres.name") != nullptr) {
      const std::optional<Genre> found = findByName(genre.name);
      if (!found.has_value()) {
        Logging::write(
            Info,
            "Db::ArtistRepository::save",
            "Failed to find genre with name: " + genre.name + ". This Error Should Not Happen."
        );
        return 0;
      } else {
        Logging::write(
            Info,
            "Db::ArtistRepository::save",
            "Not Saving Genre " + genre.name + ". Already exists with Id " + std::to_string(found.value().id) + " - SQLite UNIQUE constraint failed: genres.name"
        );
        return found.value().id;
      }
    }
  } else {
    Logging::write(
      Info,
      "Db::GenreRepository::save",
      "Saved Genre: " + genre.name
    );
  }

  return static_cast<ID>(sqlite3_last_insert_rowid(*db));
}

ID GenreRepository::save(const GenreWithTrackId& genreWithTrackId) const {
  const ID genreId = save(genreWithTrackId.genre);

  if (join(genreId, genreWithTrackId.trackId) != OK)
    Logging::write(
        Error,
        "Db::GenreRepository::save(genreWithTrackId)",
        "Unable to join GenreId " + std::to_string(genreId) + " to TrackId " + std::to_string(genreWithTrackId.trackId)
    );

  return genreId;
}

std::optional<Genre> GenreRepository::findByName(const std::string& name) const {
  const std::string query = R"sql(
    select * from genres g
    where g.name = ?;
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::GenreRepository::findByName",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return std::optional<Genre>();
  }

  sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

  if (sqlite3_step(stmt) == SQLITE_ROW)
    return std::optional<Genre>(Genre::deser(stmt));

  return std::optional<Genre>();
}

Result GenreRepository::join(const ID genreId, const ID trackId) const {
  const std::string joinQuery = R"sql(
    insert into track_to_genres (trackId, genreId)
    values (?, ?);
  )sql";

  sqlite3_stmt* joinStmt;
  if (sqlite3_prepare_v2(*db, joinQuery.c_str(), -1, &joinStmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::GenreRepository::join(genre, track)",
      "Failed to prepare join statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return ERROR;
  }

  sqlite3_bind_int(joinStmt, 1, trackId);
  sqlite3_bind_int(joinStmt, 2, genreId);

  if (sqlite3_step(joinStmt) != SQLITE_DONE) {
    Logging::write(
      Error,
      "Db::GenreRepository::join(genre, track)",
      "Failed to join GenreId " + std::to_string(genreId) + " to TrackId " + std::to_string(trackId) + ". Message: " + std::string(sqlite3_errmsg(*db))
    );
    return ERROR;
  }

  Logging::write(
    Info,
    "Db::EffectRepository::save",
    "Joined GenreId " + std::to_string(genreId) + " to TrackId " + std::to_string(trackId) + ". Message: " + std::string(sqlite3_errmsg(*db))
  );

  return OK;
}

} // Db
} // Gj
