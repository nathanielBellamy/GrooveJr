//
// Created by ns on 5/28/25.
//

#include "SceneRepository.h"

namespace Gj {
namespace Db {

SceneRepository::SceneRepository(sqlite3** db, AppState* gAppState)
  : db(db)
  , gAppState(gAppState)
  {}

int SceneRepository::save(Scene scene) const {
  const std::string query = R"sql(
    insert into scenes (index, name, version)
    values (?, ?, ?)
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "SceneRepository::save",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  sqlite3_bind_int(stmt, 1, scene.index);
  sqlite3_bind_text(stmt, 2, scene.name.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 3, scene.version + 1);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    Logging::write(
      Error,
      "SceneRepository::save",
      "Failed to save Scene " + scene.name + " at index " + std::to_string(scene.index) + " Message: " + std::string(sqlite3_errmsg(*db))
    );
  } else {
    Logging::write(
      Info,
      "EffectRepository::save",
      "Saved " + scene.name + " at index " + std::to_string(scene.index)
    );
  }

  return sqlite3_last_insert_rowid(*db);
}

} // Db
} // Gj