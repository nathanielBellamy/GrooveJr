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
  std::string title;
  Year year = 0;

  Album(std::string title)
      : id(0)
      , title(title)
      {}

  Album(std::string title, Year year)
      : id(0)
      , title(title)
      , year(year)
      {}

  Album(ID id, std::string title, Year year)
      : id(id)
      , title(title)
      , year(year)
      {}
};

} // Db
} // Gj

#endif //ALBUM_H
