//
// Created by ns on 10/6/25.
//

#ifndef ALBUM_H
#define ALBUM_H

#include <string>

#include <sqlite3.h>

#include "../../Types.h"
#include "../../../types/AtomicStr.h"

namespace Gj {
namespace Db {

struct Album {
  ID id;
  AtomicStr title;
  Year year = 0;

  Album(const std::string& title)
      : id(0)
      , title(title)
      {}

  Album(const std::string& title, Year year)
      : id(0)
      , title(title)
      , year(year)
      {}

  Album(ID id, const std::string& title, Year year)
      : id(id)
      , title(title)
      , year(year)
      {}

  static Album deser(sqlite3_stmt* stmt);
};

} // Db
} // Gj

#endif //ALBUM_H
