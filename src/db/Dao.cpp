//
// Created by ns on 5/25/25.
//

#include "Dao.h"

namespace Gj {
namespace Db {

Dao::Dao(AppState* gAppState)
  : db(nullptr)
  , gAppState(gAppState)
  , appStateRepository(&db, gAppState)
  , channelRepository(&db, gAppState)
  , effectRepository(&db, gAppState)
  , sceneRepository(&db, gAppState)
  , trackRepository(&db, gAppState)
  {

  if (initDb() == 0) {
    if (initSchema() == 0) {
      Logging::write(
        Info,
        "Db::Dao::constructor",
        "Initialized Db and constructed Dao"
      );
      // insertTestData();
      // const auto resAppState = appStateRepository.get();
      // const auto resTrack = trackRepository.getAll();
      // const auto resEffect = effectRepository.getAll();
      // const auto resScene = sceneRepository.getAll();
      // const auto resSceneEffects = sceneRepository.getEffects(0);
    }
  }
}

Dao::~Dao() {
// TODO: debug warning about deleting forward decl
//  delete db;
}

int Dao::initDb() {
  const std::filesystem::path cwd = std::filesystem::current_path();
  const std::string db_name = cwd.string() + "/groovejr.db";

  if (sqlite3_open(db_name.c_str(), &db)) {
    Logging::write(
      Critical,
      "Db::Dao::initDb",
      "Unable to init groovejr.db"
    );
    return 1;
  }

  Logging::write(
    Info,
    "Db::Dao::initDb",
    "Initialized groovejr.db"
  );
  return 0;
}

int Dao::initSchema() const {
  const std::string query = R"sql(
    create table if not exists appState (
      id integer primary key autoincrement,
      audioFramesPerBuffer integer,
      sceneId integer,
      sceneIndex integer,
      createdAt datetime default current_timestamp
    );

    -- mixer
    create table if not exists scenes (
      id integer primary key autoincrement,
      sceneIndex integer not null,
      name text not null,
      version integer not null,
      createdAt datetime default current_timestamp
    );

    create table if not exists channels (
      id integer primary key autoincrement,
      channelIndex integer not null,
      name text not null default 'Channel',
      gain real not null default 1.0,
      mute real not null default 0.0,
      solo real not null default 0.0,
      pan real not null default 0.0,
      gainL real not null default 1.0,
      gainR real not null default 1.0,
      muteL real not null default 0.0,
      muteR real not null default 0.0,
      soloL real not null default 0.0,
      soloR real not null default 0.0,
      panL real not null default 0.0,
      panR real not null default 0.0,
      createdAt datetime default current_timestamp
    );

    create table if not exists scene_to_channels (
      sceneId integer not null,
      channelId integer not null,
      primary key (sceneId, channelId),
      foreign key (sceneId) references scenes(id) on delete cascade,
      foreign key (channelId) references channels(id) on delete cascade
    );

    create table if not exists effects (
      id integer primary key autoincrement,
      filePath text not null,
      format text not null,
      name text not null,
      channelIndex integer not null,
      effectIndex integer not null,
      audioHostComponentState blob,
      audioHostControllerState blob,
      editorHostComponentState blob,
      editorHostControllerState blob,
      createdAt datetime default current_timestamp
    );

    create table if not exists channel_to_effects (
      channelId integer not null,
      effectId integer not null,
      primary key (channelId, effectId),
      foreign key (channelId) references channels(id) on delete cascade,
      foreign key (effectId) references effects(id) on delete cascade
    );

    create table if not exists scene_to_effects (
      sceneId integer not null,
      effectId integer not null,
      primary key (sceneId, effectId),
      foreign key (sceneId) references scenes(id) on delete cascade,
      foreign key (effectId) references effects(id) on delete cascade
    );

    -- music library
    create table if not exists tracks (
      id integer primary key autoincrement,
      filePath text not null,
      title text,
      bitRate integer not null,

      sf_frames integer not null,
      sf_sampleRate integer not null,
      sf_channels integer not null,
      sf_format integer not null,
      sf_sections integer not null,
      sf_seekable integer not null,
      createdAt datetime default current_timestamp
    );
  )sql";

  if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
    Logging::write(
        Critical,
        "Db::Dao::initSchema",
        "Unable to provision the database. Message: " + std::string(sqlite3_errmsg(db))
    );
    return 1;
  }

  Logging::write(
      Info,
      "Db::Dao::initSchema",
      "Provisioned the database."
  );
  return 0;
}

} // Db
} // Gj