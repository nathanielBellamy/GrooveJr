//
// Created by ns on 5/25/25.
//

#ifndef TRACK_H
#define TRACK_H

#include <iostream>
#include <string>
#include <vector>

#include <sndfile.h>
#include <sqlite3.h>

#include "../../../enums/AudioCodec.h"
#include "../../Types.h"

namespace Gj {
namespace Db {

struct Track {
  ID id;
  ID albumId;
  std::string title;
  TrackNumber trackNumber;

  Track();
  Track(ID id, ID albumId, const std::string& title, TrackNumber trackNumber);
  Track(ID albumId, const std::string& title, TrackNumber trackNumber);

  static Track empty() {
    return Track(0, 0, "- Empty Track -", 0);
  }

  static Track deser(sqlite3_stmt* stmt);
};

} // Db
} // Gj

#endif //TRACK_H
