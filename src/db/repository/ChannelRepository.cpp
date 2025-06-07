//
// Created by ns on 6/7/25.
//

#include "ChannelRepository.h"

namespace Gj {
namespace Db {

ChannelRepository::ChannelRepository(sqlite3** db, AppState* gAppState)
  : db(db)
  , gAppState(gAppState)
  {}

int ChannelRepository::save(const ChannelEntity& channel) const {
  const std::string query = R"sql(
    insert into channels (channelIndex, name, gain, mute, solo, pan, gainL, gainR, muteL, muteR, soloL, soloR, panL, panR))
    values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
  )sql";

  sqlite3_stmt* stmt;
  if (sqlite3_prepare_v2(*db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::ChannelRepository::save",
      "Failed to prepare statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  sqlite3_bind_int(stmt, 1, channel.channelIndex);
  sqlite3_bind_text(stmt, 2, channel.name.c_str(), -1, SQLITE_STATIC);
  sqlite3_bind_double(stmt, 3, channel.gain);
  sqlite3_bind_double(stmt, 4, channel.mute);
  sqlite3_bind_double(stmt, 5, channel.solo);
  sqlite3_bind_double(stmt, 6, channel.pan);
  sqlite3_bind_double(stmt, 7, channel.gainL);
  sqlite3_bind_double(stmt, 8, channel.gainR);
  sqlite3_bind_double(stmt, 9, channel.muteL);
  sqlite3_bind_double(stmt, 10, channel.muteR);
  sqlite3_bind_double(stmt, 11, channel.soloL);
  sqlite3_bind_double(stmt, 12, channel.soloR);
  sqlite3_bind_double(stmt, 13, channel.panL);
  sqlite3_bind_double(stmt, 14, channel.panR);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    Logging::write(
      Error,
      "Db::ChannelRepository::save",
      "Failed to save Channel " + channel.name + " at index " + std::to_string(channel.channelIndex) + ". Message: " + std::string(sqlite3_errmsg(*db))
    );
  } else {
    Logging::write(
      Info,
      "Db::ChannelRepository::save",
      "Saved Channel " + channel.name + " at index " + std::to_string(channel.channelIndex)
    );
  }

  const int channelId = sqlite3_last_insert_rowid(*db);

  const std::string joinQuery = R"sql(
    insert into scene_to_channels (sceneId, channelId)
    values (?, ?);
  )sql";

  sqlite3_stmt* joinStmt;
  if (sqlite3_prepare_v2(*db, joinQuery.c_str(), -1, &joinStmt, nullptr) != SQLITE_OK) {
    Logging::write(
      Error,
      "Db::ChannelRepository::save",
      "Failed to prepare join statement. Message: " + std::string(sqlite3_errmsg(*db))
    );
    return 0;
  }

  sqlite3_bind_int(joinStmt, 1, gAppState->getSceneId());
  sqlite3_bind_int(joinStmt, 2, channelId);

  if (sqlite3_step(joinStmt) != SQLITE_DONE) {
    Logging::write(
      Error,
      "Db::EffectRepository::save",
      "Failed to join Channel " + channel.name + " id: " + std::to_string(channelId) + " to sceneId: " + std::to_string(gAppState->getSceneId()) + ". Message: " + std::string(sqlite3_errmsg(*db))
    );
  } else {
    Logging::write(
      Info,
      "Db::EffectRepository::save",
      "Joined Channel " + channel.name + " id: " + std::to_string(channelId) + " to sceneId " + std::to_string(gAppState->getSceneId())
    );
  }

  return channelId;
}

} // Db
} // Gj