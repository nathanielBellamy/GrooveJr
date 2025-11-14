//
// Created by ns on 10/6/25.
//

#include "Artist.h"

namespace Gj {
namespace Db {

Artist Artist::deser(sqlite3_stmt* stmt) {
  const ID id = sqlite3_column_int(stmt, 0);
  const unsigned char* name = sqlite3_column_text(stmt, 1);

  return Artist(
    id,
    AtomicStr(reinterpret_cast<const char*>(name))
  );
}

Artist Artist::unknown() {
  return Artist(" - Unknown Artist - ");
}

} // Db
} // Gj
