//
// Created by ns on 10/6/25.
//

#ifndef ARTIST_H
#define ARTIST_H

#include <string>

#include <sqlite3.h>

#include "../../Types.h"

namespace Gj {
namespace Db {

struct Artist {
  ID id;
  std::string name;

  Artist(const ID id, const std::string& name)
      : id(id)
      , name(name)
      {}

  Artist(const std::string& name)
      : id(0)
      , name(name)
      {}

  static Artist deser(sqlite3_stmt* stmt);

  static Artist unknown();
};

} // Db
} // Gj



#endif //ARTIST_H
