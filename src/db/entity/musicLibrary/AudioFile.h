//
// Created by ns on 10/6/25.
//

#ifndef AUDIOFILE_H
#define AUDIOFILE_H

#include <string>

#include <sndfile.h>

#include "../../Types.h"
#include "../../Logging.h"
#include "../../../enums/AudioCodec.h"
#include "../../../enums/Result.h"



namespace Gj {
namespace Db {

struct AudioFile {
  ID id;
  std::string filePath;
  SF_INFO sfInfo;
  bool isLossless;
  bool isVariableBitRate;
  AudioCodec audioCodec;

  AudioFile(const std::string& filePath)
    : id(0)
    , filePath(filePath)
    {
    if (getSfInfo() != OK)
      Logging::write(
        Error,
        "Db::AudioFile::AudioFile(filePath)",
        "An Error Occurred while reading SF_INFO for filepath: " + filePath
      );
  }

  AudioFile(const ID id, const std::string& filePath)
    : id(id)
    , filePath(filePath)
    {
    if (getSfInfo() != OK)
      Logging::write(
        Error,
        "Db::AudioFile::AudioFile(id, filePath)",
        "An Error Occurred while reading SF_INFO for id: " + std::to_string(id) + " filepath: " + filePath
      );
  }

  AudioFile(const ID id, const std::string& filePath, const SF_INFO& sfInfo)
    : id(id)
    , filePath(filePath)
    , sfInfo(sfInfo)
    {}

  Result getSfInfo() {
    // TODO

    return OK;
  }
};

} // Db
} // Gj


#endif //AUDIOFILE_H
