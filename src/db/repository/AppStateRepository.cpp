//
// Created by ns on 5/29/25.
//

#include "AppStateRepository.h"

namespace Gj {
namespace Db {

AppStateRepository::AppStateRepository(sqlite3** db, AppState* gAppState)
  : db(db)
  , gAppState(gAppState)
  {}

Db::AppState AppStateRepository::get() {
  const std::string query = R"sql(
    select * from appState
    order by createdAt desc
    limit 1;
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::AppStateRepository::get",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return Db::AppState(256, 0, 0);
  }

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const auto appState = Db::AppState::deser(stmt);
    return appState;
  }

  return Db::AppState(256, 0, 0);
}

int AppStateRepository::save() {

  return 0;
}

} // Db
} // Gj