//
// Created by ns on 5/25/25.
//

#include "Database.h"

namespace Gj {
namespace Db {

Database::Database() {
  init();
  provision();
}

Database::~Database() {
  delete db;
}

void Database::init() {
  const std::filesystem::path cwd = std::filesystem::current_path();
  const std::string db_name = cwd.string() + "/groovejr.db";

  if (sqlite3_open(db_name.c_str(), &db)) {
    Logging::write(
      Critical,
      "main::initSql",
      "Unable to init groovejr.db"
    );
  } else {
    Logging::write(
      Info,
      "main::initSql",
      "Initialized groovejr.db"
    );
  }
}

void Database::provision() {
  std::string query = R"sql(
    create table if not exists tracks (
      id integer primary key autoincrement,
      file_path text not null,
      title text,
      sf_frames integer not null,
      sf_samplerate integer not null,
      sf_channels integer not null,
      created_at datetime default current_timestamp
    )

    insert into tracks (file_path, title, sf_frames, sf_samplerate, sf_channels)
    values (
      '/Users/ns/Music/Amy Winehouse/Back to Black/Amy Winehouse - Back to Black (2006) [FLAC]/06 Love Is A Losing Game.flac',
      'Love is a Losing Game',
      1234,
      44100,
      2
    )
  )sql";

  if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
    Logging::write(
        Critical,
        "Db::Database::provision",
        "Unable to provision the database: " + std::string(sqlite3_errmsg(db))
    );
  } else {
    Logging::write(
        Critical,
        "Db::Database::provision",
        "Provisioned the database."
    );
  }
}

} // Db
} // Gj