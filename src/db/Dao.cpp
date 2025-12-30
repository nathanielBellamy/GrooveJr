//
// Created by ns on 5/25/25.
//

#include "Dao.h"

namespace Gj {
namespace Db {
Dao::Dao(State::Core* stateCore)
: db(nullptr)
  , stateCore(stateCore)
  , appStateRepository(&db, stateCore)
  , channelRepository(&db, stateCore)
  , pluginRepository(&db, stateCore)
  , sceneRepository(&db, stateCore)
  , albumRepository(&db, stateCore)
  , artistRepository(&db, stateCore)
  , audioFileRepository(&db, stateCore)
  , cacheRepository(&db, stateCore)
  , genreRepository(&db, stateCore)
  , playlistRepository(&db, stateCore)
  , queueRepository(&db, stateCore)
  , trackRepository(&db, stateCore) {
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
      // const auto resPlugin = pluginRepository.getAll();
      // const auto resScene = sceneRepository.getAll();
      // const auto resScenePlugins = sceneRepository.getPlugins(0);
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
      sceneVersion integer,
      crossfade integer,
      currentlyPlaying integer,
      createdAt datetime default current_timestamp,
      foreign key (sceneId) references scenes(id) on delete cascade
    );

    -- mixer
    create table if not exists scenes (
      id integer primary key autoincrement,
      sceneId integer not null,
      name text not null,
      playbackSpeed real not null default 1.0,
      version integer not null,
      createdAt datetime default current_timestamp,
      unique (sceneId, version)
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

    create table if not exists plugins (
      id integer primary key autoincrement,
      filePath text not null,
      format text not null,
      name text not null,
      channelIndex integer not null,
      pluginIndex integer not null,
      audioHostComponentState blob,
      audioHostControllerState blob,
      editorHostComponentState blob,
      editorHostControllerState blob,
      createdAt datetime default current_timestamp
    );

    create table if not exists channel_to_plugins (
      channelId integer not null,
      pluginId integer not null,
      primary key (channelId, pluginId),
      foreign key (channelId) references channels(id) on delete cascade,
      foreign key (pluginId) references plugins(id) on delete cascade
    );

    create table if not exists scene_to_plugins (
      sceneId integer not null,
      pluginId integer not null,
      primary key (sceneId, pluginId),
      foreign key (sceneId) references scenes(id) on delete cascade,
      foreign key (pluginId) references plugins(id) on delete cascade
    );

    -- music library
    create table if not exists albums (
      id integer primary key autoincrement,
      title text not null,
      year integer not null,
      createdAt datetime default current_timestamp
    );

    create table if not exists artists (
      id integer primary key autoincrement,
      name text not null unique,
      createdAt datetime default current_timestamp
    );

    create table if not exists playlists (
      id integer primary key autoincrement,
      name text not null unique,
      createdAt datetime default current_timestamp
    );

    create table if not exists tracks (
      id integer primary key autoincrement,
      albumId integer not null,
      title text not null,
      trackNumber integer not null,
      createdAt datetime default current_timestamp,
      foreign key (albumId) references albums(id) on delete cascade,
      unique (albumId, trackNumber)
    );

    create table if not exists audioFiles (
      id integer primary key autoincrement,
      trackId integer not null,
      filePath text not null unique,
      valid int default 0,
      sf_frames integer not null,
      sf_sampleRate integer not null,
      sf_channels integer not null,
      sf_format integer not null,
      sf_sections integer not null,
      sf_seekable integer not null,
      createdAt datetime default current_timestamp,
      foreign key (trackId) references tracks(id) on delete cascade
    );

    create table if not exists genres (
      id integer primary key autoincrement,
      name text not null unique,
      createdAt datetime default current_timestamp
    );

    create table if not exists queue (
      id integer primary key autoincrement,
      audioFileId integer not null,
      trackNumber integer not null unique,
      createdAt datetime default current_timestamp,
      foreign key (audioFileId) references audioFiles(id) on delete cascade
    );

    create table if not exists cache (
      id integer primary key autoincrement,
      audioFileId integer not null,
      createdAt datetime default current_timestamp,
      foreign key (audioFileId) references audioFiles(id) on delete cascade
    );

    create table if not exists track_to_artists (
      trackId integer not null,
      artistId integer not null,
      primary key (trackId, artistId),
      foreign key (trackId) references tracks(id) on delete cascade,
      foreign key (artistId) references artists(id) on delete cascade
    );

    create table if not exists track_to_genres (
      trackId integer not null,
      genreId integer not null,
      primary key (trackId, genreId),
      foreign key (trackId) references tracks(id) on delete cascade,
      foreign key (genreId) references genres(id) on delete cascade
    );

    create table if not exists audioFile_to_playlists (
      audioFileId integer not null,
      playlistId integer not null,
      trackNumber integer not null,
      primary key (audioFileId, playlistId),
      foreign key (audioFileId) references audioFiles(id) on delete cascade,
      foreign key (playlistId) references playlists(id) on delete cascade,
      unique (playlistId, trackNumber)
    );

    create table if not exists artist_to_albums (
      artistId integer not null,
      albumId integer not null,
      primary key (artistId, albumId),
      foreign key (artistId) references artists(id) on delete cascade,
      foreign key (albumId) references albums(id) on delete cascade
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
