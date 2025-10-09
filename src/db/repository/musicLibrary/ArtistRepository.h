//
// Created by ns on 10/6/25.
//

#ifndef ARTISTREPOSITORY_H
#define ARTISTREPOSITORY_H

#include <cstring>
#include <vector>

#include <sqlite3.h>

#include "../../Logging.h"
#include "../../../AppState.h"

#include "../../entity/musicLibrary/Artist.h"
#include "../../Types.h"

namespace Gj {
namespace Db {

class ArtistRepository final {

  sqlite3** db;
  AppState* gAppState;

  public:
    ArtistRepository(sqlite3** db, AppState* gAppState);
    std::vector<Artist> getAll() const;
    ID save(const Artist& artist) const;
    ID saveAll(const std::vector<Artist>& artist) const;
    Artist findByName(const std::string& name) const;

};

} // Db
} // Gj



#endif //ARTISTREPOSITORY_H
