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
    return 0;
  }

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    Logging::write(
      Error,
      "Db::CacheRepository::clear",
      "Failed to clear Cache : "
      "Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  Logging::write(
    Info,
    "Db::CacheRepository::clear",
    "Cleared Cache"
  );
  return OK;
}

ID CacheRepository::save(const Cache& cache) const {
  const std::string query = R"sql(
    insert into cache (audioFileId)
    values (?)
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::CacheRepository::save",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  sqlite3_bind_int(stmt, 1, cache.audioFileId);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    Logging::write(
      Error,
      "Db::CacheRepository::save",
      "Failed to save Cache audioFileId: " + std::to_string(cache.audioFileId) +
        " Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  Logging::write(
    Info,
    "Db::CacheRepository::save",
    "Saved Cache. audioFileId: " + std::to_string(cache.audioFileId)
  );
  return static_cast<ID>(sqlite3_last_insert_rowid(*db));
}

} // Db
} // Gj