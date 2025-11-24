//
// Created by ns on 10/13/25.
//

#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <iostream>
#include <string>
#include <vector>

#include <sqlite3.h>

#include "../../../enums/AudioCodec.h"
#include "../../../types/Types.h"
#include "../../../types/AtomicStr.h"

namespace Gj {
namespace Db {

struct Playlist {
  ID id;
  AtomicStr name;

  Playlist(ID id, const AtomicStr& name);
  Playlist(const AtomicStr& name);

  static Playlist empty() {
    return Playlist("- Empty Playlist -");
  }

  static Playlist deser(sqlite3_stmt* stmt);
};

} // Db
} // Gj

#endif //PLAYLIST_H
