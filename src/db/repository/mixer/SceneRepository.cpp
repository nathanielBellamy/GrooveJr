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

int SceneRepository::sceneCount() const {
  const std::string query = R"sql(
    select count(*) from scenes
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
    ? sqlite3_column_int(stmt, 0)
    : 0;
}

std::optional<SceneID> SceneRepository::nextSceneId() const {
  const std::string query = R"sql(
    select sceneId from scenes
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
    return std::nullopt;
  }

  if (sqlite3_step(stmt) == SQLITE_ROW)
    return std::optional(
      static_cast<SceneID>(
        sqlite3_column_int(stmt, 0) + 1
      )
    );

  if (sceneCount() == 0)
    return std::optional(1ULL);

  return std::nullopt;
}


ID SceneRepository::create(const Scene& scene) const {
  const std::string query = R"sql(
    insert into scenes (sceneId, version, name, playbackSpeed)
    values (?, ?, ?, ?)
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

  SceneID sceneId;
  if (scene.sceneId) {
    sceneId = scene.sceneId;
  } else if (const auto nsid = nextSceneId(); nsid.has_value()) {
    sceneId = nsid.value();
  } else {
    Logging::write(
      Error,
      "Db::SceneRepository::create",
      "Unable to determine nextSceneId."
    );
    return 0;
  }

  sqlite3_bind_int(stmt, 1, sceneId);
  sqlite3_bind_int(stmt, 2, scene.version);
  sqlite3_bind_text(stmt, 3, scene.name.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_double(stmt, 4, scene.playbackSpeed);

  if (sqlite3_step(stmt) != SQLITE_DONE)
    Logging::write(
      Error,
      "Db::SceneRepository::create",
      "Failed to save Scene " + scene.name + " Message: " + std::string(sqlite3_errmsg(*db))
    );
  else
    Logging::write(
      Info,
      "Db::SceneRepository::create",
      "Saved " + scene.name
    );

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

std::vector<ChannelEntity> SceneRepository::getChannels(const ID sceneId) const {
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

std::vector<Plugin> SceneRepository::getPlugins(const ID sceneId) const {
  std::vector<Plugin> plugins;
  const std::string query = R"sql(
    select * from plugins p
    right join scene_to_plugins stp
    on ste.pluginId = p.id
    where stp.sceneId = ?;
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::SceneRepository::getPlugins",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return plugins;
  }

  sqlite3_bind_int(stmt, 1, sceneId);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const auto plugin = Plugin::deser(stmt);
    plugins.push_back(plugin);
  }

  return plugins;
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