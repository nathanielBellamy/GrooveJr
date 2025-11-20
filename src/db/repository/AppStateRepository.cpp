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
    Logging::write(
      Info,
      "Db::AppStateRepository::get",
      "Loaded app state."
    );
    const auto appState = AppStateEntity::deser(stmt);
    return appState;
  }

  Logging::write(
    Error,
    "Db::AppStateRepository::get",
    "Failed to load app state."
  );
  return AppStateEntity::base();
}

ID AppStateRepository::save() const {
  const std::string query = R"sql(
    insert into appState (audioFramesPerBuffer, sceneId, crossfade)
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
  sqlite3_bind_int(stmt, 2, gAppState->getSceneDbId());
  sqlite3_bind_int(stmt, 3, gAppState->getCrossfade());

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

Result AppStateRepository::persistAndSet() const {
  try {
    save();

    // reload
    const auto appStateEntity = get();
    const Scene scene = findScene(appStateEntity.sceneId).value_or(Scene::base());

    gAppState->setFromEntityAndScene(appStateEntity, scene);

    Logging::write(
      Info,
      "Db::AppStateRepository::persistAndSet",
      "Persisted AppState: " + gAppState->toString()
    );

    return OK;
  } catch (const std::exception& e) {
    Logging::write(
      Error,
      "Db::AppStateRepository::persistAndSet",
      "Failed to persist AppState: " + std::string(e.what())
    );
    return ERROR;
  }
}

std::optional<Scene> AppStateRepository::findScene(const ID id) const {
  const std::string query = R"sql(
    select * from scenes s
    where s.id = ?
    limit 1;
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::SceneRepository::find",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return std::nullopt;
  }

  sqlite3_bind_int(stmt, 1, id);

  if (sqlite3_step(stmt) == SQLITE_ROW)
    return std::optional(Scene::deser(stmt));

  Logging::write(
    Warning,
    "Db::SceneRepository::find",
    "Unable to find dbID " + std::to_string(id)
  );
  return std::nullopt;
}

} // Db
} // Gj