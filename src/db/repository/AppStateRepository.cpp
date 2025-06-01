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

AppStateEntity AppStateRepository::get() const {
  const std::string query = R"sql(
    select * from appState
    order by id desc
    limit 1;
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::AppStateRepository::get",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return AppStateEntity::base();
  }

  if (sqlite3_step(stmt) == SQLITE_ROW) {
    const auto appState = AppStateEntity::deser(stmt);
    return appState;
  }

  return AppStateEntity::base();
}

int AppStateRepository::save() const {
  const std::string query = R"sql(
    insert into appState (audioFramesPerBuffer, sceneId, sceneIndex)
    values (?, ?, ?)
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::AppStateRepositoryRepository::save",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  sqlite3_bind_int(stmt, 1, gAppState->getAudioFramesPerBuffer());
  sqlite3_bind_int(stmt, 2, gAppState->getSceneId());
  sqlite3_bind_int(stmt, 3, gAppState->getSceneIndex());

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    Logging::write(
      Error,
      "Db::AppStateRepository::save",
      "Failed to save AppState"
    );
  } else {
    Logging::write(
      Info,
      "Db::AppStateRepository::save",
      "Saved AppState"
    );
  }

  return sqlite3_last_insert_rowid(*db);
}

} // Db
} // Gj