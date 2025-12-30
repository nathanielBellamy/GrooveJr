//
// Created by ns on 10/6/25.
//

#ifndef AUDIOFILEREPOSITORY_H
#define AUDIOFILEREPOSITORY_H

#include <optional>

#include "../../../types/AtomicStr.h"
#include "MusicLibraryRepository.h"
#include "../../entity/musicLibrary/AudioFile.h"
#include "../../dto/musicLibrary/DecoratedAudioFile.h"

namespace Gj {
namespace Db {

class AudioFileRepository final : public MusicLibraryRepository {
  public:
    AudioFileRepository(sqlite3** db, State::Core* stateCore)
      : MusicLibraryRepository(db, stateCore)
      {};

    std::vector<AudioFile> getAll() const;
    ID save(const AudioFile& audioFile) const;

    [[nodiscard]]
    std::optional<AudioFile> findByFilePath(const AtomicStr& filePath) const;

    std::optional<DecoratedAudioFile> findDecoratedAudioFileById(ID id) const;

};

} // Db
} // Gj



#endif //AUDIOFILEREPOSITORY_H
