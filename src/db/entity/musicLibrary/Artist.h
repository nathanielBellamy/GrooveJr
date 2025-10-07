//
// Created by ns on 10/6/25.
//

#ifndef ARTIST_H
#define ARTIST_H

#include <string>

#include "../../Types.h"

namespace Gj {
namespace Db {

struct Artist {
  ID id;
  std::string name;

  Artist(std::string name)
      : id(0)
      , name(name)
      {}

  Artist(ID id, std::string name)
      : id(id)
      , name(name)
      {}
};

} // Db
} // Gj



#endif //ARTIST_H
