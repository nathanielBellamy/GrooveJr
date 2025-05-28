//
// Created by ns on 5/25/25.
//

#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <filesystem>

#include <sqlite3.h>

#include "../Logging.h"
#include "../AppState.h"

#include "./repository/EffectRepository.h"
#include "./repository/TrackRepository.h"

namespace Gj {
namespace Db {

class Dao {
  sqlite3* db;
  AppState* gAppState;

  public:
    EffectRepository effectRepository;
    TrackRepository trackRepository;

    Dao(AppState* gAppState);
    ~Dao();
    int initDb();
    int initSchema() const;
    void insertTestData() const;
};

} // Db
} // Gj




#endif //DATABASE_H
