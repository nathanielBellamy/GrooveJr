//
// Created by ns on 5/26/25.
//

#include "Effect.h"

namespace Gj {
namespace Db {

Effect::Effect(
    const int id,
    const std::string& filePath,
    const std::string& format,
    const std::string& name,
    const int channelIndex,
    const int effectIndex,
    std::vector<uint8_t> audioHostComponentStateBlob,
    std::vector<uint8_t> audioHostControllerStateBlob,
    std::vector<uint8_t> editorHostComponentStateBlob,
    std::vector<uint8_t> editorHostControllerStateBlob
  )
  : id(id)
  , filePath(filePath)
  , format(format)
  , name(name)
  , channelIndex(channelIndex)
  , effectIndex(effectIndex)
  , audioHostComponentStateBlob(audioHostComponentStateBlob)
  , audioHostControllerStateBlob(audioHostControllerStateBlob)
  , editorHostComponentStateBlob(editorHostComponentStateBlob)
  , editorHostControllerStateBlob(editorHostControllerStateBlob)
  {}

Effect::Effect(
    const std::string& filePath,
    const std::string& format,
    const std::string& name,
    const int channelIndex,
    const int effectIndex,
    std::vector<uint8_t> audioHostComponentStateBlob,
    std::vector<uint8_t> audioHostControllerStateBlob,
    std::vector<uint8_t> editorHostComponentStateBlob,
    std::vector<uint8_t> editorHostControllerStateBlob
  )
  : filePath(filePath)
  , format(format)
  , name(name)
  , channelIndex(channelIndex)
  , effectIndex(effectIndex)
  , audioHostComponentStateBlob(audioHostComponentStateBlob)
  , audioHostControllerStateBlob(audioHostControllerStateBlob)
  , editorHostComponentStateBlob(editorHostComponentStateBlob)
  , editorHostControllerStateBlob(editorHostControllerStateBlob)
  {}

Effect Effect::deser(sqlite3_stmt *stmt) {
  Logging::write(
    Info,
    "Db::Effect::deser",
    "Deserializing effect"
  );

  const int id = sqlite3_column_int(stmt, 0);
  const unsigned char* filePath = sqlite3_column_text(stmt, 1);
  const unsigned char* format = sqlite3_column_text(stmt, 2);
  const unsigned char* name = sqlite3_column_text(stmt, 3);
  const int channelIndex = sqlite3_column_int(stmt, 4);
  const int effectIndex = sqlite3_column_int(stmt, 5);
  const int audioHostComponentStateBlobSize = sqlite3_column_bytes(stmt, 6);
  const void* audioHostComponentStateBlobRaw = sqlite3_column_blob(stmt, 6);
  const int audioHostControllerStateBlobSize = sqlite3_column_bytes(stmt, 7);
  const void* audioHostControllerStateBlobRaw = sqlite3_column_blob(stmt, 7);
  const int editorHostComponentStateBlobSize = sqlite3_column_bytes(stmt, 8);
  const void* editorHostComponentStateBlobRaw = sqlite3_column_blob(stmt, 8);
  const int editorHostControllerStateBlobSize = sqlite3_column_bytes(stmt, 9);
  const void* editorHostControllerStateBlobRaw = sqlite3_column_blob(stmt, 9);

  const std::vector audioHostComponentStateBlobDeser(
    static_cast<const uint8_t*>(audioHostComponentStateBlobRaw),
    static_cast<const uint8_t*>(audioHostComponentStateBlobRaw) + audioHostComponentStateBlobSize
  );
  Logging::write(
    Info,
    "Effect::deser",
    "Found audioHostComponentStateBlob of size " + std::to_string(audioHostComponentStateBlobDeser.size())
  );

  const std::vector audioHostControllerStateBlobDeser(
    static_cast<const uint8_t*>(audioHostControllerStateBlobRaw),
    static_cast<const uint8_t*>(audioHostControllerStateBlobRaw) + audioHostControllerStateBlobSize
  );
  Logging::write(
    Info,
    "Effect::deser",
    "Found audioHostControllerStateBlob of size " + std::to_string(audioHostControllerStateBlobSize)
  );

  const std::vector editorHostComponentStateBlobDeser(
    static_cast<const uint8_t*>(editorHostComponentStateBlobRaw),
    static_cast<const uint8_t*>(editorHostComponentStateBlobRaw) + editorHostComponentStateBlobSize
  );
  Logging::write(
    Info,
    "Effect::deser",
    "Found editorHostComponentStateBlob of size " + std::to_string(editorHostComponentStateBlobDeser.size())
  );

  const std::vector editorHostControllerStateBlobDeser(
    static_cast<const uint8_t*>(editorHostControllerStateBlobRaw),
    static_cast<const uint8_t*>(editorHostControllerStateBlobRaw) + editorHostControllerStateBlobSize
  );
  Logging::write(
    Info,
    "Effect::deser",
    "Found editorHostControllerStateBlob of size " + std::to_string(editorHostControllerStateBlobDeser.size())
  );

  Logging::write(
    Info,
    "Db::Effect::deser",
    "Done deserializing effect"
  );
  return {
    id,
    std::string(reinterpret_cast<const char*>(filePath)),
    std::string(reinterpret_cast<const char*>(format)),
    std::string(reinterpret_cast<const char*>(name)),
    channelIndex,
    effectIndex,
    audioHostComponentStateBlobDeser,
    audioHostControllerStateBlobDeser,
    editorHostComponentStateBlobDeser,
    editorHostControllerStateBlobDeser
  };
}

} // Db
} // Gj