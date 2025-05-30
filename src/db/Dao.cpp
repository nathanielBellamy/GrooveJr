//
// Created by ns on 5/25/25.
//

#include "Dao.h"

namespace Gj {
namespace Db {

Dao::Dao(AppState* gAppState)
  : gAppState(gAppState)
  , appStateRepository(&db, gAppState)
  , effectRepository(&db, gAppState)
  , sceneRepository(&db, gAppState)
  , trackRepository(&db, gAppState)
  {
  if (initDb() == 0) {
    if (initSchema() == 0) {
      insertTestData();
      const auto resTrack = trackRepository.getAll();
      const auto resEffect = effectRepository.getAll();
      const auto resScene = sceneRepository.getAll();
      const auto resSceneEffects = sceneRepository.getEffects(0);
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

    create table if not exists scenes (
      id integer primary key autoincrement,
      sceneIndex integer not null,
      name text not null,
      version integer not null,
      createdAt datetime default current_timestamp
    );

    create table if not exists effects (
      id integer primary key autoincrement,
      filePath text not null,
      format text not null,
      name text not null,
      state numeric,
      channelIndex integer not null,
      effectIndex integer not null,
      version integer not null,
      createdAt datetime default current_timestamp
    );

    create table if not exists scene_to_effects (
      sceneId integer not null,
      effectId integer not null,
      primary key (sceneId, effectId),
      foreign key (sceneId) references scenes(id) on delete cascade,
      foreign key (effectId) references effects(id) on delete cascade
    );

    create table if not exists tracks (
      id integer primary key autoincrement,
      filePath text not null,
      title text,
      frames integer not null,
      sampleRate integer not null,
      channels integer not null,
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

void Dao::insertTestData() const {
  const std::string query = R"sql(
    insert into effects (filePath, format, name, channelIndex, effectIndex, version)
    values (
      '/Library/Audio/Plug-Ins/VST3/FooEffect.vst3',
      'vst3',
      'Foo Bar',
      1,
      0,
      3
    );

    insert into tracks (filePath, title, frames, sampleRate, channels)
    values (
      '/foo.flac',
      'Foo Bar',
      310310,
      44100,
      2
    );
  )sql";

  if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
    Logging::write(
        Critical,
        "Db::Database::insertTestData",
        "Unable to insert test data into the database. Message: " + std::string(sqlite3_errmsg(db))
    );
  }

  sceneRepository.save(
    Scene(0, "Base Scene")
  );

  effectRepository.save(
    Effect("/Library/Audio/Plug-Ins/VST3/BarEffect.vst3", "vst3", "Bar Bar", 1234, 2, 0, 3)
  );

  Logging::write(
      Info,
      "Db::Database::insertTestData",
      "Inserted test data in the database."
  );
}

} // Db
} // Gj