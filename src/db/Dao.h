//
// Created by ns on 5/25/25.
//

#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <filesystem>

#include <sqlite3.h>

#include "../Logging.h"

#include "./repository/TrackRepository.h"

namespace Gj {
namespace Db {

class Dao {

  sqlite3* db;

  public:
    TrackRepository trackRepository;

    Dao();
    ~Dao();
    int initDb();
    int initSchema() const;
    void insertTestData() const;
};

} // Db
} // Gj




#endif //DATABASE_H
