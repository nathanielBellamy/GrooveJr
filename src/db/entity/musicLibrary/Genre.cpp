//
// Created by ns on 10/6/25.
//

#include "Genre.h"

namespace Gj {
namespace Db {

Genre Genre::deser(sqlite3_stmt* stmt) {
  const ID id = sqlite3_column_int(stmt, 0);
  const unsigned char* name = sqlite3_column_text(stmt, 1);

  return Genre(
    id,
    AtomicStr(reinterpret_cast<const char*>(name))
  );
}

} // Db
} // Gj