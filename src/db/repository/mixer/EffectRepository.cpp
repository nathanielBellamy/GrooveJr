//
// Created by ns on 5/26/25.
//

#include "EffectRepository.h"


namespace Gj {
namespace Db {

EffectRepository::EffectRepository(sqlite3** db, AppState* gAppState)
  : db(db)
  , gAppState(gAppState)
  {}

std::vector<Effect> EffectRepository::getAll() const {
  std::vector<Effect> effects;
  const std::string query = R"sql(
    select * from effects;
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::EffectRepository::getAll",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return effects;
  }

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const auto effect = Effect::deser(stmt);
    effects.push_back(effect);
  }

  return effects;
}

int EffectRepository::save(const Effect& effect) const {
  const std::string query = R"sql(
    insert into effects (
      filePath,
      format,
      name,
      channelIndex,
      effectIndex,
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
      "Db::EffectRepository::save",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  sqlite3_bind_text(stmt, 1, effect.filePath.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 2, effect.format.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 3, effect.name.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 4, effect.channelIndex);
  sqlite3_bind_int(stmt, 5, effect.effectIndex);
  sqlite3_bind_blob(
    stmt,
    6,
    effect.audioHostComponentStateBlob.data(),
    static_cast<int>(effect.audioHostComponentStateBlob.size()),
    SQLITE_TRANSIENT
  );
  sqlite3_bind_blob(
    stmt,
    7,
    effect.audioHostControllerStateBlob.data(),
    static_cast<int>(effect.audioHostControllerStateBlob.size()),
    SQLITE_TRANSIENT
  );
  sqlite3_bind_blob(
    stmt,
    8,
    effect.editorHostComponentStateBlob.data(),
    static_cast<int>(effect.editorHostComponentStateBlob.size()),
    SQLITE_TRANSIENT
  );
  sqlite3_bind_blob(
    stmt,
    9,
    effect.editorHostControllerStateBlob.data(),
    static_cast<int>(effect.editorHostControllerStateBlob.size()),
    SQLITE_TRANSIENT
  );

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    Logging::write(
      Error,
      "Db::EffectRepository::save",
      "Failed to save Effect " + effect.name + " at index " + std::to_string(effect.effectIndex) + " on channel " + std::to_string(effect.channelIndex) + ". Message: " + std::string(sqlite3_errmsg(*db))
    );
  } else {
    Logging::write(
      Info,
      "Db::EffectRepository::save",
      "Saved " + effect.name + " at index " + std::to_string(effect.effectIndex) + " on channel " + std::to_string(effect.channelIndex)
    );
  }

  const int effectId = sqlite3_last_insert_rowid(*db);

  const std::string joinQuery = R"sql(
    insert into scene_to_effects (sceneId, effectId)
    values (?, ?);
  )sql";

  sqlite3_stmt* joinStmt;
  if (sqlite3_prepare_v2(*db, joinQuery.c_str(), -1, &joinStmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::EffectRepository::save",
      "Failed to prepare join statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  sqlite3_bind_int(joinStmt, 1, gAppState->getSceneId());
  sqlite3_bind_int(joinStmt, 2, effectId);

  if (sqlite3_step(joinStmt) != SQLITE_DONE) {
    Logging::write(
      Error,
      "Db::EffectRepository::save",
      "Failed to join Effect " + effect.name + " id: " + std::to_string(effectId) + " to sceneId: " + std::to_string(gAppState->getSceneId()) + ". Message: " + std::string(sqlite3_errmsg(*db))
    );
  } else {
    Logging::write(
      Info,
      "Db::EffectRepository::save",
      "Joined Effect " + effect.name + " id: " + std::to_string(effectId) + " to sceneId " + std::to_string(gAppState->getSceneId())
    );
  }

  return effectId;
}

std::vector<Effect> EffectRepository::getBySceneId(const int sceneId) const {
  std::vector<Effect> effects;
  const std::string query = R"sql(
    select * from effects e
    right join scene_to_effects ste
    on e.id = ste.effectId
    where ste.sceneId = ?;
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::EffectRepository::getBySceneId",
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

std::vector<Effect> EffectRepository::getBySceneIndex(const int sceneIndex) const {
  std::vector<Effect> effects;
  const std::string query = R"sql(
    select * from effects e
    right join scene_to_effects ste
    on e.id = ste.effectId
    where ste.sceneId = (
      select id from scenes
      where sceneIndex = ?
      order by id desc
      limit 1
    );
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::EffectRepository::getBySceneId",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return effects;
  }

  sqlite3_bind_int(stmt, 1, sceneIndex);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const auto effect = Effect::deser(stmt);
    effects.push_back(effect);
  }

  return effects;
}

} // Db
} // Gj