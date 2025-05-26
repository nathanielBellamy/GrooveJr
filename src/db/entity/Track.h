//
// Created by ns on 5/25/25.
//

#ifndef TRACK_H
#define TRACK_H

#include <string>

#include <sqlite3.h>

namespace Gj {
namespace Db {

struct Track {
  int id;
  std::string filePath;
  std::string title;
  int sf_frames;
  int sf_samplerate;
  int sf_channels;

  Track(int id, std::string filePath, std::string title);
  static Track deser(sqlite3_stmt* stmt);
};

} // Db
} // Gj

#endif //TRACK_H
