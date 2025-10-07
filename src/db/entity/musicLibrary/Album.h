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
  Year year;

};

} // Db
} // Gj

#endif //ALBUM_H
