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
    scenes.push_back(scene);
  }

  return scenes;
}

int SceneRepository::save(const Scene& scene) const {
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

  return static_cast<int>(sqlite3_last_insert_rowid(*db));
}

std::vector<ChannelEntity> SceneRepository::getChannels(const int sceneId) const {
  std::vector<ChannelEntity> channels;
  const std::string query = R"sql(
    select * from channels c
    right join scene_to_channels stc
    on stc.channelId = c.id
    where stc.sceneId = ?;
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::SceneRepository::getChannels",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return channels;
  }

  sqlite3_bind_int(stmt, 1, sceneId);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const auto channel = ChannelEntity::deser(stmt);
    channels.push_back(channel);
  }

  return channels;
}

std::vector<Effect> SceneRepository::getEffects(const int sceneId) const {
  std::vector<Effect> effects;
  const std::string query = R"sql(
    select * from effects e
    right join scene_to_effects ste
    on ste.effectId = e.id
    where ste.sceneId = ?;
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

  sqlite3_bind_int(stmt, 1, sceneId);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const auto effect = Effect::deser(stmt);
    effects.push_back(effect);
  }

  return effects;
}

std::optional<Scene> SceneRepository::find(const ID sceneId) const {
  // find
  const std::string query = R"sql(
    select *
    from scenes
    where id = ?
    limit 1
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

  sqlite3_bind_int(stmt, 1, sceneId);

  if (sqlite3_step(stmt) == SQLITE_ROW) {
    const auto scene = Scene::deser(stmt);
    Logging::write(
      Info,
      "Db::SceneRepository::find",
      "Found scene sceneId: " + std::to_string(scene.id) + " sceneIndex: " + std::to_string(scene.sceneIndex)
    );
    return std::optional(scene);
  }

  Logging::write(
    Error,
    "Db::SceneRepository::find",
    "Unable to find scene sceneId: " + std::to_string(sceneId)
  );
  return std::nullopt;
}

int SceneRepository::findOrCreateBySceneIndex(const int sceneIndex) const {
  // find
  const std::string query = R"sql(
    select *
    from scenes
    where sceneIndex = ?
    order by id desc
    limit 1
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::SceneRepository::findOrCreateBySceneIndex",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  sqlite3_bind_int(stmt, 1, sceneIndex);

  if (sqlite3_step(stmt) == SQLITE_ROW) {
    const auto scene = Scene::deser(stmt);
    Logging::write(
      Info,
      "Db::SceneRepository::findOrCreateBySceneIndex",
      "Found scene sceneId: " + std::to_string(scene.id) + " sceneIndex: " + std::to_string(scene.sceneIndex)
    );
    return scene.id;
  }

  // or create
  const std::string insertQuery = R"sql(
    insert into scenes (sceneIndex, name, version)
    values (?, "Empty Scene", 1);
  )sql";

  sqlite3_stmt* insertStmt;
  if (sqlite3_prepare_v2(*db, insertQuery.c_str(), -1, &insertStmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::SceneRepository::findOrCreateBySceneIndex",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  sqlite3_bind_int(insertStmt, 1, sceneIndex);

  if (sqlite3_step(insertStmt) == SQLITE_DONE)
    return static_cast<int>(sqlite3_last_insert_rowid(*db));

  Logging::write(
    Error,
    "Db::SceneRepository::findOrCreateBySceneIndex",
    "Failed to create sceneIndex: " + std::to_string(sceneIndex)
  );
  return 0;
}


} // Db
} // Gj