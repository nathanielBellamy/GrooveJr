//
// Created by ns on 10/6/25.
//

#include "AudioFile.h"

namespace Gj {
namespace Db {

AudioFile AudioFile::deser(sqlite3_stmt* stmt) {
  const ID id = sqlite3_column_int(stmt, 0);
  const ID trackId = sqlite3_column_int(stmt, 1);
  const unsigned char* filePath = sqlite3_column_text(stmt, 2);
  const int valid = sqlite3_column_int(stmt, 3);
  const sf_count_t sf_frames = sqlite3_column_int(stmt, 4);
  const int sf_sampleRate = sqlite3_column_int(stmt, 5);
  const int sf_channels = sqlite3_column_int(stmt, 6);
  const int sf_format = sqlite3_column_int(stmt, 7);
  const int sf_sections = sqlite3_column_int(stmt, 8);
  const int sf_seekable = sqlite3_column_int(stmt, 9);

  SF_INFO sfInfo {
      sf_frames,
      sf_sampleRate,
      sf_channels,
      sf_format,
      sf_sections,
      sf_seekable
  };

  return AudioFile(
      id,
      trackId,
      AtomicStr(reinterpret_cast<const char*>(filePath)),
      valid ? true : false,
      sfInfo
  );
}

Result AudioFile::getSfInfo() {
  // https://svn.ict.usc.edu/svn_vh_public/trunk/lib/vhcl/libsndfile/doc/api.html
  // > When opening a file for read, the format field should be set to zero before calling sf_open().
  sfInfo.format = 0;
  const auto file = sf_open(filePath.c_str(), SFM_READ, &sfInfo);

  if (file == nullptr) {
    Logging::write(
      Error,
      "Db::AudioFile::getSfInfo",
      "Unable to open input filePath: " + filePath + "Error: " + std::string(sf_strerror(nullptr))
    );

    sf_close(file);
    valid = false;
    return ERROR;
  };


  sf_close(file);
  valid = true;
  return OK;
}

} // Db
} // Gj