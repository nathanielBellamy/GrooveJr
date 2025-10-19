//
// Created by ns on 10/19/25.
//

#include "QueueRepository.h"

namespace Gj {
namespace Db {

ID QueueRepository::save(const Queue& queue) const {
  const std::string query = R"sql(
    insert into queue (audioFileId, trackNumber)
    values (?, ?)
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::QueueRepository::save",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  sqlite3_bind_int(stmt, 1, queue.audioFileId);
  sqlite3_bind_int(stmt, 2, queue.trackNumber);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    const char* errmsg = sqlite3_errmsg(*db);

    Logging::write(
      Error,
      "Db::QueueRepository::save",
      "Failed to save Queue audioFileId: " + std::to_string(queue.audioFileId) + " Message: " + std::string(sqlite3_errmsg(*db))
    );
  } else {
    Logging::write(
      Info,
      "Db::QueueRepository::save",
      "Saved Queue. audioFileId: " + std::to_string(queue.audioFileId) + " trackNumber: " + std::to_string(queue.trackNumber)
    );
  }

  return static_cast<ID>(sqlite3_last_insert_rowid(*db));
}

} // Db
} // Gj