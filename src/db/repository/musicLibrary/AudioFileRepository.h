//
// Created by ns on 10/6/25.
//

#ifndef AUDIOFILEREPOSITORY_H
#define AUDIOFILEREPOSITORY_H

#include <optional>

#include "MusicLibraryRepository.h"
#include "../../entity/musicLibrary/AudioFile.h"

namespace Gj {
namespace Db {

class AudioFileRepository final : public MusicLibraryRepository {
  public:
    AudioFileRepository(sqlite3** db, AppState* gAppState)
      : MusicLibraryRepository(db, gAppState)
      {};

    std::vector<AudioFile> getAll() const;
    ID save(const AudioFile& audioFile) const;

    [[nodiscard]]
    std::optional<AudioFile> findByFilePath(const std::string& filePath) const;

    std::optional<DecoratedAudioFile> findDecoratedAudioFileById(ID id) const;

};

} // Db
} // Gj



#endif //AUDIOFILEREPOSITORY_H
