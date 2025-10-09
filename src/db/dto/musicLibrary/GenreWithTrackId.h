//
// Created by ns on 10/9/25.
//

#ifndef GENREWITHTRACKID_H
#define GENREWITHTRACKID_H

#include "../../entity/musicLibrary/Genre.h"
#include "../../Types.h"

namespace Gj {
namespace Db {

struct GenreWithTrackId {
  Genre genre;
  ID trackId;
};

} // Db
} // Gj

#endif //GENREWITHTRACKID_H
