//
// Created by ns on 10/6/25.
//

#ifndef TRACKDECORATE_H
#define TRACKDECORATE_H

#include "../../Logging.h"
#include "../../entity/musicLibrary/AudioFile.h"
#include "../../entity/musicLibrary/Track.h"

namespace Gj {
namespace Db {

class TrackDecorate final {

  public:
    TrackDecorate(
      const std::string& filePath,
      const std::string& title,
      const std::string& artist,
      const std::string& album,
      uint16_t trackNumber
    );

  private:
    unsigned long long id;
    Track track;
    AudioFile audioFile;
    std::string title;
    std::string artist;
    std::string album;
    unsigned int trackNumber;
    unsigned int year;
};

} // Db
} // Gj

#endif //TRACKDECORATE_H
