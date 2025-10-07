//
// Created by ns on 10/6/25.
//

#ifndef ALBUM_H
#define ALBUM_H

#include <string>

#include "../../Types.h"

namespace Gj {
namespace Db {

struct Album {
  ID id;
  std::string name;
  Year year = 0;

  Album(std::string name)
      : id(0)
      , name(name)
      {}

  Album(std::string name, Year year)
      : id(0)
      , name(name)
      , year(year)
      {}

  Album(ID id, std::string name, Year year)
      : id(id)
      , name(name)
      , year(year)
      {}
};

} // Db
} // Gj

#endif //ALBUM_H
