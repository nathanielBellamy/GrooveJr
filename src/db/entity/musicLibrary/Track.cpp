//
// Created by ns on 5/25/25.
//

#include "Track.h"

namespace Gj {
namespace Db {

Track::Track(const int id, const std::string& title)
  : id(id)
  , title(title)
  {}

Track Track::deser(sqlite3_stmt* stmt) {
  const int id = sqlite3_column_int(stmt, 0);
  // TODO: update sql
  // const unsigned char* filePath = sqlite3_column_text(stmt, 1);
  const unsigned char* title = sqlite3_column_text(stmt, 2);

  return Track(
    id,
    std::string(reinterpret_cast<const char*>(title))
  );
}

} // Db
} // Gj