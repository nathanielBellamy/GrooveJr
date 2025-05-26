//
// Created by ns on 5/25/25.
//

#ifndef TRACK_H
#define TRACK_H

#include <string>

namespace Gj {
namespace Db {

class Track {
  int id;
  std::string filePath;
  std::string title;
  int sf_frames;
  int sf_samplerate;
  int sf_channels;

  public:
    Track();
};

} // Db
} // Gj

#endif //TRACK_H
