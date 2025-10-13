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
#include "../../Types.h"

namespace Gj {
namespace Db {

struct Playlist {
  ID id;
  std::string name;
};

} // Db
} // Gj

#endif //PLAYLIST_H
