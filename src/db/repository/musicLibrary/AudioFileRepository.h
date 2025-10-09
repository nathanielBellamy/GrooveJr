//
// Created by ns on 10/6/25.
//

#ifndef AUDIOFILEREPOSITORY_H
#define AUDIOFILEREPOSITORY_H

#include <optional>

#include <sqlite3.h>

#include "../../Logging.h"
#include "../../../AppState.h"

#include "../../entity/musicLibrary/AudioFile.h"

namespace Gj {
namespace Db {

class AudioFileRepository {
  sqlite3** db;
  AppState* gAppState;

  public:
    AudioFileRepository(sqlite3** db, AppState* gAppState)
      : db(db)
      , gAppState(gAppState)
      {};
    std::vector<AudioFile> getAll() const;
    ID save(const AudioFile& audioFile) const;

    [[nodiscard]]
    std::optional<AudioFile> findByFilePath(const std::string& filePath) const;

};

} // Db
} // Gj



#endif //AUDIOFILEREPOSITORY_H
