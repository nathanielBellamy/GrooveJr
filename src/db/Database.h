//
// Created by ns on 5/25/25.
//

#ifndef DATABASE_H
#define DATABASE_H

#include <filesystem>

#include <sqlite3.h>

#include "../Logging.h"

namespace Gj {
namespace Db {

class Database {

  sqlite3* db;

  public:
    Database();
    ~Database();
    void init();
    void provision();
};

} // Db
} // Gj




#endif //DATABASE_H
