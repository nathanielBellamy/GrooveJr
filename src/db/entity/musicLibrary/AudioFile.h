//
// Created by ns on 10/6/25.
//

#ifndef AUDIOFILE_H
#define AUDIOFILE_H

#include <string>

#include <sndfile.h>

namespace Gj {
namespace Db {

struct AudioFile {

  std::string filePath;
  SF_INFO sfInfo;

  AudioFile(const std::string& filePath)
    : filePath(filePath)
    {}
};

} // Db
} // Gj


#endif //AUDIOFILE_H
