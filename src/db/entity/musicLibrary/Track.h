//
// Created by ns on 5/25/25.
//

#ifndef TRACK_H
#define TRACK_H

#include <iostream>
#include <string>
#include <sndfile.h>

#include <sqlite3.h>

#include "../../../enums/AudioCodec.h"

namespace Gj {
namespace Db {

struct Track {
  uint64_t id;
  std::string filePath;
  std::string title;
  uint64_t duration;
  uint16_t trackNumber;
  uint32_t bitRate;

  sf_count_t sf_frames;
  int sf_samplerate;
  int sf_channels;
  int sf_format;
  int sf_sections;
  int sf_seekable;

  bool isLossless;
  bool isVariableBitRate;
  AudioCodec audioCodec;

  Track(int id, const std::string& filePath, const std::string& title);
  static Track deser(sqlite3_stmt* stmt);
};

} // Db
} // Gj

#endif //TRACK_H
