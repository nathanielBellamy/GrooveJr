//
// Created by ns on 10/29/25.
//

#ifndef CACHEREPOSITORY_H
#define CACHEREPOSITORY_H

#include <vector>

#include "MusicLibraryRepository.h"

#include "../../../enums/Result.h"
#include "../../entity/musicLibrary/Cache.h"

namespace Gj {
namespace Db {

class CacheRepository final : public MusicLibraryRepository {

  public:
    CacheRepository(sqlite3** db, State::Core* stateCore)
      : MusicLibraryRepository(db, stateCore)
      {};

    Result save(const std::vector<Cache>& caches) const;
    Result clear() const;
};


} // Db
} // Gj

#endif //CACHEREPOSITORY_H
