//
// Created by ns on 10/6/25.
//

#ifndef AUDIOFILE_H
#define AUDIOFILE_H

#include <string>

#include <sndfile.h>
#include <sqlite3.h>

#include "../../Types.h"
#include "../../Logging.h"
#include "../../../enums/AudioCodec.h"
#include "../../../enums/Result.h"

namespace Gj {
namespace Db {

struct AudioFile {
  ID id;
  ID trackId;
  std::string filePath;
  SF_INFO sfInfo;
  bool valid = false;
  // bool isLossless;
  // bool isVariableBitRate;
  // AudioCodec audioCodec;

  AudioFile()
    : id(0)
    , trackId(0)
    , filePath("")
    {
    Logging::write(
      Info,
      "Db::AudioFile::AudioFile()",
      "Instantiated Blank AudioFile"
    );
  }

  AudioFile(const std::string& filePath)
    : id(0)
    , trackId(0)
    , filePath(filePath)
    {
    if (getSfInfo() != OK)
      Logging::write(
        Error,
        "Db::AudioFile::AudioFile(filePath)",
        "An Error Occurred while reading SF_INFO for filepath: " + filePath
      );
  }

  AudioFile(const ID trackId, const std::string& filePath)
    : id(0)
    , trackId(trackId)
    , filePath(filePath)
    {
    if (getSfInfo() != OK)
      Logging::write(
        Error,
        "Db::AudioFile::AudioFile(id, filePath)",
        "An Error Occurred while reading SF_INFO for id: " + std::to_string(id) + " filepath: " + filePath
      );
  }

  AudioFile(const ID id, const ID trackId, const std::string& filePath, const bool valid, const SF_INFO& sfInfo)
    : id(id)
    , trackId(trackId)
    , filePath(filePath)
    , sfInfo(sfInfo)
    , valid(valid)
    {}

  Result getSfInfo();
  static AudioFile deser(sqlite3_stmt* stmt);
};

} // Db
} // Gj


#endif //AUDIOFILE_H
