//
// Created by ns on 10/6/25.
//

#include "Album.h"

namespace Gj {
namespace Db {

Album Album::deser(sqlite3_stmt* stmt) {
  const ID id = sqlite3_column_int(stmt, 0);
  const unsigned char* title = sqlite3_column_text(stmt, 1);
  const Year year = sqlite3_column_int(stmt, 2);

  return Album(
      id,
      AtomicStr(reinterpret_cast<const char*>(title)),
      year
  );
}

} // Db
} // Gj