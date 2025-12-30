//
// Created by ns on 10/19/25.
//

#ifndef QUEUEREPOSITORY_H
#define QUEUEREPOSITORY_H

#include "MusicLibraryRepository.h"

#include "../../entity/musicLibrary/Queue.h"

namespace Gj {
namespace Db {

class QueueRepository final : public MusicLibraryRepository {

  public:
    QueueRepository(sqlite3** db, State::Core* stateCore)
      : MusicLibraryRepository(db, stateCore)
      {};

    ID save(const Queue& queue) const;
    TrackNumber nextTrackNumber() const;
    TrackNumber maxTrackNumber() const;
};


} // Db
} // Gj


#endif //QUEUEREPOSITORY_H
