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
  const sf_count_t sf_frames = sqlite3_column_int(stmt, 3);
  const int sf_sampleRate = sqlite3_column_int(stmt, 4);
  const int sf_channels = sqlite3_column_int(stmt, 5);
  const int sf_format = sqlite3_column_int(stmt, 6);
  const int sf_sections = sqlite3_column_int(stmt, 7);
  const int sf_seekable = sqlite3_column_int(stmt, 8);

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
      std::string(reinterpret_cast<const char*>(filePath)),
      sfInfo
  );
}

} // Db
} // Gj