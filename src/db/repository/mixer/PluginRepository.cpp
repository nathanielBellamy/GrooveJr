//
// Created by ns on 5/26/25.
//

#include "PluginRepository.h"


namespace Gj {
namespace Db {

PluginRepository::PluginRepository(sqlite3** db, AppState* gAppState)
  : db(db)
  , gAppState(gAppState)
  {}

std::vector<Plugin> PluginRepository::getAll() const {
  std::vector<Plugin> plugins;
  const std::string query = R"sql(
    select * from plugins;
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::PluginRepository::getAll",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return plugins;
  }

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const auto plugin = Plugin::deser(stmt);
    plugins.push_back(plugin);
  }

  return plugins;
}

int PluginRepository::save(const Plugin& plugin) const {
  const std::string query = R"sql(
    insert into plugins (
      filePath,
      format,
      name,
      channelIndex,
      pluginIndex,
      audioHostComponentState,
      audioHostControllerState,
      editorHostComponentState,
      editorHostControllerState
    )
    values (?, ?, ?, ?, ?, ?, ?, ?, ?)
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::PluginRepository::save",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  sqlite3_bind_text(stmt, 1, plugin.filePath.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 2, plugin.format.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 3, plugin.name.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 4, plugin.channelIndex);
  sqlite3_bind_int(stmt, 5, plugin.pluginIndex);
  sqlite3_bind_blob(
    stmt,
    6,
    plugin.audioHostComponentStateBlob.data(),
    static_cast<int>(plugin.audioHostComponentStateBlob.size()),
    SQLITE_TRANSIENT
  );
  sqlite3_bind_blob(
    stmt,
    7,
    plugin.audioHostControllerStateBlob.data(),
    static_cast<int>(plugin.audioHostControllerStateBlob.size()),
    SQLITE_TRANSIENT
  );
  sqlite3_bind_blob(
    stmt,
    8,
    plugin.editorHostComponentStateBlob.data(),
    static_cast<int>(plugin.editorHostComponentStateBlob.size()),
    SQLITE_TRANSIENT
  );
  sqlite3_bind_blob(
    stmt,
    9,
    plugin.editorHostControllerStateBlob.data(),
    static_cast<int>(plugin.editorHostControllerStateBlob.size()),
    SQLITE_TRANSIENT
  );

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    Logging::write(
      Error,
      "Db::PluginRepository::save",
      "Failed to save Plugin " + plugin.name + " at index " + std::to_string(plugin.pluginIndex) + " on channel " + std::to_string(plugin.channelIndex) + ". Message: " + std::string(sqlite3_errmsg(*db))
    );
  } else {
    Logging::write(
      Info,
      "Db::PluginRepository::save",
      "Saved " + plugin.name + " at index " + std::to_string(plugin.pluginIndex) + " on channel " + std::to_string(plugin.channelIndex)
    );
  }

  const ID pluginId = sqlite3_last_insert_rowid(*db);

  const std::string joinQuery = R"sql(
    insert into scene_to_plugins (sceneId, pluginId)
    values (?, ?);
  )sql";

  sqlite3_stmt* joinStmt;
  if (sqlite3_prepare_v2(*db, joinQuery.c_str(), -1, &joinStmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::PluginRepository::save",
      "Failed to prepare join statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  sqlite3_bind_int(joinStmt, 1, gAppState->getSceneDbId());
  sqlite3_bind_int(joinStmt, 2, pluginId);

  if (sqlite3_step(joinStmt) != SQLITE_DONE) {
    Logging::write(
      Error,
      "Db::PluginRepository::save",
      "Failed to join Plugin " + plugin.name + " id: " + std::to_string(pluginId) + " to sceneDbId: " + std::to_string(gAppState->getSceneDbId()) + ". Message: " + std::string(sqlite3_errmsg(*db))
    );
  } else {
    Logging::write(
      Info,
      "Db::PluginRepository::save",
      "Joined Plugin " + plugin.name + " id: " + std::to_string(pluginId) + " to sceneDbId " + std::to_string(gAppState->getSceneDbId())
    );
  }

  return pluginId;
}

std::vector<Plugin> PluginRepository::getBySceneId(const int sceneId) const {
  std::vector<Plugin> plugins;
  const std::string query = R"sql(
    select * from plugins p
    right join scene_to_plugins stp
    on p.id = stp.pluginId
    where ste.sceneId = ?;
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::PluginRepository::getBySceneId",
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

} // Db
} // Gj