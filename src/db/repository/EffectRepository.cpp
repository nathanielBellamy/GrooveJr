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
      "EffectRepository::getAll",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return effects;
  }

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const auto effect = Effect::deser(stmt);
    std::cout << "Effect Row: ID = " << effect.id << ", filePath = " << effect.filePath << ", format = " << effect.format << ", name = " << effect.name << ", version = " << effect.version << std::endl;
    effects.push_back(effect);
  }

  return effects;
}

int EffectRepository::save(Effect effect) const {
  const std::string query = R"sql(
    insert into effects (filePath, format, name, state, channelIndex, effectIndex, version)
    values (?, ?, ?, ?, ?, ?, ?)
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "EffectRepository::save",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  sqlite3_bind_text(stmt, 1, effect.filePath.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 2, effect.format.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_text(stmt, 3, effect.name.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_int(stmt, 4, effect.state);
  sqlite3_bind_int(stmt, 5, effect.channelIndex);
  sqlite3_bind_int(stmt, 6, effect.effectIndex);
  sqlite3_bind_int(stmt, 7, effect.version + 1);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    Logging::write(
      Error,
      "EffectRepository::save",
      "Failed to save Effect " + effect.name + " at index " + std::to_string(effect.effectIndex) + " on channel " + std::to_string(effect.channelIndex) + " Message: " + std::string(sqlite3_errmsg(*db))
    );
  } else {
    Logging::write(
      Info,
      "EffectRepository::save",
      "Saved " + effect.name + " at index " + std::to_string(effect.effectIndex) + " on channel " + std::to_string(effect.channelIndex)
    );
  }

  return sqlite3_last_insert_rowid(*db);
}

} // Db
} // Gj