//
// Created by ns on 5/29/25.
//

#ifndef APPSTATEENTITY_H
#define APPSTATEENTITY_H

#include <sqlite3.h>

namespace Gj {
namespace Db {


struct AppStateEntity {
  int audioFramesPerBuffer;
  int sceneId;
  int sceneIndex;

  AppStateEntity(int audioFramesPerBuffer, int sceneId, int sceneIndex);

  static AppStateEntity deser(sqlite3_stmt* stmt);

  static AppStateEntity base();
};

} // Db
} // Gj

#endif //APPSTATE_H
