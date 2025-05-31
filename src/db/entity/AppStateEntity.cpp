//
// Created by ns on 5/29/25.
//

#include "AppStateEntity.h"

namespace Gj {
namespace Db {

AppStateEntity::AppStateEntity(const int audioFramesPerBuffer, const int sceneId, const int sceneIndex)
  : audioFramesPerBuffer(audioFramesPerBuffer)
  , sceneId(sceneId)
  , sceneIndex(sceneIndex)
  {}

AppStateEntity AppStateEntity::deser(sqlite3_stmt* stmt) {
  const int audioFramesPerBuffer = sqlite3_column_int(stmt, 0);
  const int sceneId = sqlite3_column_int(stmt, 1);
  const int sceneIndex = sqlite3_column_int(stmt, 2);

  return AppStateEntity(audioFramesPerBuffer, sceneId, sceneIndex);
}

AppStateEntity AppStateEntity::base() {
  return AppStateEntity(256, 0, 0);
}

} // Db
} // Gj