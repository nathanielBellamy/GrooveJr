//
// Created by ns on 10/13/25.
//

#include "Playlist.h"

namespace Gj {
namespace Db {

Playlist::Playlist(const ID id, const std::string& name)
  : id(id)
  , name(name)
  {}

Playlist::Playlist(const std::string& name)
  : id(0)
  , name(name)
  {}

Playlist Playlist::deser(sqlite3_stmt* stmt) {
  const ID id = sqlite3_column_int(stmt, 0);
  const unsigned char* name  = sqlite3_column_text(stmt, 1);

  return Playlist(
    id,
    std::string(reinterpret_cast<const char*>(name))
  );
}

} // Db
} // Gj
