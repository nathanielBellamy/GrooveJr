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

SceneID SceneRepository::nextSceneId() const {
  const std::string query = R"sql(
    select id from scenes
    order by sceneId desc
    limit 1
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::SceneRepository::nextSceneId",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  return (sqlite3_step(stmt) == SQLITE_ROW)
    ? sqlite3_column_int(stmt, 0) + 1
    : 0;
}


ID SceneRepository::create(const Scene& scene) const {
  const std::string query = R"sql(
    insert into scenes (sceneId, version, name, playbackSpeed)
    values (?, 0, ?, ?)
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::SceneRepository::create",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  sqlite3_bind_int(stmt, 1, nextSceneId());
  sqlite3_bind_text(stmt, 2, scene.name.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_double(stmt, 3, scene.playbackSpeed);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    Logging::write(
      Error,
      "Db::SceneRepository::create",
      "Failed to save Scene " + scene.name + " Message: " + std::string(sqlite3_errmsg(*db))
    );
  } else {
    Logging::write(
      Info,
      "Db::SceneRepository::create",
      "Saved " + scene.name
    );
  }

  return static_cast<ID>(sqlite3_last_insert_rowid(*db));
}

std::optional<Scene> SceneRepository::update(Scene scene) const {
  scene.version++;
  scene.id = create(scene);

  if (scene.id == 0) {
    Logging::write(
      Error,
      "Db::SceneRepository::update",
      "Failed to update Scene sceneId: " + std::to_string(scene.sceneId)
    );
    return std::nullopt;
  }

  Logging::write(
    Info,
    "Db::SceneRepository::update",
    "Updated Scene sceneId " + std::to_string(scene.sceneId) +
    " now version: " + std::to_string(scene.version) + " dbId: " + std::to_string(scene.id)
  );

  return std::optional(scene);
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

std::optional<Scene> SceneRepository::find(const ID dbId) const {
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

  sqlite3_bind_int(stmt, 1, dbId);

  if (sqlite3_step(stmt) == SQLITE_ROW) {
    const auto scene = Scene::deser(stmt);
    Logging::write(
      Info,
      "Db::SceneRepository::find",
      "Found scene sceneId: " + std::to_string(scene.id)
    );
    return std::optional(scene);
  }

  Logging::write(
    Error,
    "Db::SceneRepository::find",
    "Unable to find scene dbId: " + std::to_string(dbId)
  );
  return std::nullopt;
}

Scene SceneRepository::findOrCreate(const ID dbId) const {
  if (const auto found = find(dbId)) {
    return found.value();
  }

  auto base = Scene::base();
  const auto newId = create(base);
  base.id = newId;
  return base;
}

std::optional<Scene> SceneRepository::findBySceneId(const SceneID sceneId) const {
  // find
  const std::string query = R"sql(
    select *
    from scenes s
    where s.sceneId = ?
    order by s.sceneId desc
    limit 1
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::SceneRepository::findBySceneId",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return std::nullopt;
  }

  sqlite3_bind_int(stmt, 1, sceneId);

  if (sqlite3_step(stmt) == SQLITE_ROW) {
    const auto scene = Scene::deser(stmt);
    Logging::write(
      Info,
      "Db::SceneRepository::findBySceneId",
      "Found scene sceneId: " + std::to_string(scene.id)
    );
    return std::optional(scene);
  }

  Logging::write(
    Error,
    "Db::SceneRepository::find",
    "Unable to find scene dbId: " + std::to_string(sceneId)
  );
  return std::nullopt;
}

} // Db
} // Gj