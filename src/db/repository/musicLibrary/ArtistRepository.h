//
// Created by ns on 10/6/25.
//

#ifndef ARTISTREPOSITORY_H
#define ARTISTREPOSITORY_H

#include <vector>

#include "../../../types/AtomicStr.h"
#include "MusicLibraryRepository.h"
#include "../../entity/musicLibrary/Artist.h"
#include "../../Types.h"

namespace Gj {
namespace Db {

class ArtistRepository final : public MusicLibraryRepository {

  public:
    ArtistRepository(sqlite3** db, AppState* gAppState)
      : MusicLibraryRepository(db, gAppState)
      {};
    std::vector<Artist> getAll() const;
    ID save(const Artist& artist) const;
    ID saveAll(const std::vector<Artist>& artist) const;
    Artist findByName(const AtomicStr& name) const;

};

} // Db
} // Gj



#endif //ARTISTREPOSITORY_H
