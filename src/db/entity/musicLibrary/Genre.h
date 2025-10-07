//
// Created by ns on 10/6/25.
//

#ifndef GENRE_H
#define GENRE_H

#include <string>

#include "../../Types.h"

namespace Gj {
namespace Db {

struct Genre {
  ID id;
  std::string name;

  Genre(std::string name)
      : id(0)
      , name(name)
      {};

  Genre(ID id, std::string name)
      : id(id)
      , name(name)
      {};
};

} // Db
} // Gj


#endif //GENRE_H
