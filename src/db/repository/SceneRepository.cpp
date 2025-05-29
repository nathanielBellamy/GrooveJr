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

std::vector<Scene> SceneRepository::getAll() const {
  std::vector<Scene> scenes;
  const std::string query = R"sql(
    select * from scenes;
  )sql";

  sqlite3_stmt* stmt;

  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::SceneRepository::getAll",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return scenes;
  }

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const auto scene = Scene::deser(stmt);
    std::cout << "Scene: id = " << scene.id << ", sceneIndex = " << scene.sceneIndex << ", name = " << scene.name << ", version = " << scene.version << std::endl;
    scenes.push_back(scene);
  }

  return scenes;
}

int SceneRepository::save(Scene scene) const {
  const std::string query = R"sql(
    insert into scenes (sceneIndex, name, version)
    values (?, ?, ?)
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::SceneRepository::save",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  sqlite3_bind_int(stmt, 1, scene.sceneIndex);
  sqlite3_bind_text(stmt, 2, scene.name.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 3, scene.version + 1);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    Logging::write(
      Error,
      "Db::SceneRepository::save",
      "Failed to save Scene " + scene.name + " at index " + std::to_string(scene.sceneIndex) + " Message: " + std::string(sqlite3_errmsg(*db))
    );
  } else {
    Logging::write(
      Info,
      "Db::SceneRepository::save",
      "Saved " + scene.name + " at index " + std::to_string(scene.sceneIndex)
    );
  }

  return sqlite3_last_insert_rowid(*db);
}

std::vector<Effect> SceneRepository::getEffects(const int sceneIndex) const {
  std::vector<Effect> effects;
  // TODO: versioning
  const std::string query = R"sql(
    select * from effects e
    right join scene_to_effects ste
    on ste.effectId = e.id
    where ste.sceneId = (
      select id
      from scenes
      where sceneIndex = ?
      limit 1
    );
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::SceneRepository::getEffects",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return effects;
  }

  sqlite3_bind_int(stmt, 1, sceneIndex);


  std::cout << "============== scene get effects ============" << std::endl;
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const auto effect = Effect::deser(stmt);
    std::cout << "Row: ID = " << effect.id << ", filePath = " << effect.filePath << ", name = " << effect.name << std::endl;
    effects.push_back(effect);
  }

  return effects;
}

} // Db
} // Gj