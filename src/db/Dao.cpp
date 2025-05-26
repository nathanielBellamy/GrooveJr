//
// Created by ns on 5/25/25.
//

#include "Dao.h"

namespace Gj {
namespace Db {

Dao::Dao()
  : trackRepository(&db)
  {
  if (init() == 0) {
    if (provision() == 0) {
      insertTestData();
      const auto res = trackRepository.getAll();
    }
  }
}

Dao::~Dao() {
// TODO: debug warning about deleting forward decl
//  delete db;
}

int Dao::init() {
  const std::filesystem::path cwd = std::filesystem::current_path();
  const std::string db_name = cwd.string() + "/groovejr.db";

  if (sqlite3_open(db_name.c_str(), &db)) {
    Logging::write(
      Critical,
      "main::initSql",
      "Unable to init groovejr.db"
    );
    return 1;
  }

  Logging::write(
    Info,
    "main::initSql",
    "Initialized groovejr.db"
  );
  return 0;
}

int Dao::provision() const {
  std::string query = R"sql(
    create table if not exists tracks (
      id integer primary key autoincrement,
      file_path text not null,
      title text,
      sf_frames integer not null,
      sf_samplerate integer not null,
      sf_channels integer not null,
      created_at datetime default current_timestamp
    );

  )sql";

  if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
    Logging::write(
        Critical,
        "Db::Database::provision",
        "Unable to provision the database. Message: " + std::string(sqlite3_errmsg(db))
    );
    return 1;
  }

  Logging::write(
      Critical,
      "Db::Database::provision",
      "Provisioned the database."
  );
  return 0;
}

void Dao::insertTestData() const {
  std::string query = R"sql(
    insert into tracks (file_path, title, sf_frames, sf_samplerate, sf_channels)
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

  Logging::write(
      Critical,
      "Db::Database::insertTestData",
      "Inserted test data in the database."
  );
}

} // Db
} // Gj