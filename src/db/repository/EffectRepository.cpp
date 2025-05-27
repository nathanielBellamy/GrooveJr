//
// Created by ns on 5/26/25.
//

#include "EffectRepository.h"


namespace Gj {
namespace Db {

EffectRepository::EffectRepository(sqlite3** db)
  : db(db)
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
    insert into effects (
  )sql";


}

} // Db
} // Gj