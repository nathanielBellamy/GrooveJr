//
// Created by ns on 5/29/25.
//

#ifndef APPSTATEENTITY_H
#define APPSTATEENTITY_H

#include <sqlite3.h>
#include <sndfile.h>
#include <jack/jack.h>

#include "../Types.h"

namespace Gj {
namespace Db {

struct AppStateEntity {
  ID id;
  jack_nframes_t audioFramesPerBuffer;
  ID sceneId;
  sf_count_t crossfade;
  ID currentlyPlaying;

  AppStateEntity(
    ID id,
    jack_nframes_t audioFramesPerBuffer,
    ID sceneId,
    sf_count_t crossfade,
    ID currentlyPlaying
  );

  static AppStateEntity deser(sqlite3_stmt* stmt);

  static AppStateEntity base();
};

} // Db
} // Gj

#endif //APPSTATE_H
