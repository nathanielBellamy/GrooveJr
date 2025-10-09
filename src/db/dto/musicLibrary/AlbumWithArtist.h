//
// Created by ns on 10/8/25.
//

#ifndef ALBUMWITHARTISTS_H
#define ALBUMWITHARTISTS_H

#include "../../entity/musicLibrary/Album.h"
#include "../../entity/musicLibrary/Artist.h"

namespace Gj {
namespace Db {

struct AlbumWithArtist {
  Album album;
  Artist artist;
};

} // Db
} // Gj

#endif //ALBUMWITHARTIST_H
