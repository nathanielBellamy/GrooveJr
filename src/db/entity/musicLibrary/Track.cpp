//
// Created by ns on 5/25/25.
//

#include "Track.h"

namespace Gj {
namespace Db {

Track::Track(const ID id, const ID albumId, const std::string& title, const TrackNumber trackNumber)
  : id(id)
  , albumId(albumId)
  , title(title)
  , trackNumber(trackNumber)
  {}

Track::Track(const ID albumId, const std::string& title, const TrackNumber trackNumber)
  : id(0)
  , albumId(albumId)
  , title(title)
  , trackNumber(trackNumber)
  {}

Track Track::deser(sqlite3_stmt* stmt) {
  const ID id = sqlite3_column_int(stmt, 0);
  const ID albumId = sqlite3_column_int(stmt, 1);
  const unsigned char* title = sqlite3_column_text(stmt, 2);
  const TrackNumber trackNumber = sqlite3_column_int(stmt, 3);;

  return Track(
    id,
    albumId,
    std::string(reinterpret_cast<const char*>(title)),
    trackNumber
  );
}

} // Db
} // Gj