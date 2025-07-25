//
// Created by ns on 5/29/25.
//

#include "AppStateEntity.h"

namespace Gj {
namespace Db {

AppStateEntity::AppStateEntity(const int id, const jack_nframes_t audioFramesPerBuffer, const int sceneId, const int sceneIndex)
  : id(id)
  , audioFramesPerBuffer(audioFramesPerBuffer)
  , sceneId(sceneId)
  , sceneIndex(sceneIndex)
  {}

AppStateEntity AppStateEntity::deser(sqlite3_stmt* stmt) {
  const int id = sqlite3_column_int(stmt, 0);
  const int audioFramesPerBuffer = sqlite3_column_int(stmt, 1);
  const int sceneId = sqlite3_column_int(stmt, 2);
  const int sceneIndex = sqlite3_column_int(stmt, 3);

  return {
    id,
    static_cast<jack_nframes_t>(audioFramesPerBuffer),
    sceneId,
    sceneIndex
  };
}

AppStateEntity AppStateEntity::base() {
  return {
    0, 256, 0, 0
  };
}

} // Db
} // Gj