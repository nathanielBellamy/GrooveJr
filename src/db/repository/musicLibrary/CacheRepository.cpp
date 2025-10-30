//
// Created by ns on 10/29/25.
//

#include "CacheRepository.h"

namespace Gj {
namespace Db {

Result CacheRepository::clear() const {
  const std::string query = R"sql(
    delete from cache
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::CacheRepository::clear",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return ERROR;
  }

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    Logging::write(
      Error,
      "Db::CacheRepository::clear",
      "Failed to clear Cache : "
      "Message: " + std::string(sqlite3_errmsg(*db))
    );
    return ERROR;
  }

  Logging::write(
    Info,
    "Db::CacheRepository::clear",
    "Cleared Cache"
  );
  return OK;
}

Result CacheRepository::save(const std::vector<Cache>& caches) const {
  if (clear() == ERROR) {
    Logging::write(
        Error,
        "Db::CacheRepository::save",
        "Failed to clear Cache"
    );
    return ERROR;
  }

  std::string query = "insert into cache (audioFileId) values ";

  for (int i = 0; i < caches.size(); i++) {
    query += "(" + std::to_string(caches.at(i).audioFileId) + ")";
    if (i < caches.size() - 1)
      query += ", ";
  }

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::CacheRepository::save",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return ERROR;
  }

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    Logging::write(
      Error,
      "Db::CacheRepository::save",
      "Failed to save Cache : "
        " Message: " + std::string(sqlite3_errmsg(*db))
    );
    return ERROR;
  }

  Logging::write(
    Info,
    "Db::CacheRepository::save",
    "Saved Cache."
  );
  return OK;
}

} // Db
} // Gj