//
// Created by ns on 10/6/25.
//

#ifndef GENRE_H
#define GENRE_H

#include <sqlite3.h>

#include "../../../types/Types.h"
#include "../../../types/AtomicStr.h"

namespace Gj {
namespace Db {
struct Genre {
  ID id;
  AtomicStr name;

  explicit Genre(const AtomicStr& name)
  : id(0)
    , name(name) {
  };

  Genre(const ID id, const AtomicStr& name)
  : id(id)
    , name(name) {
  };

  static Genre deser(sqlite3_stmt* stmt);
};
} // Db
} // Gj


#endif //GENRE_H
