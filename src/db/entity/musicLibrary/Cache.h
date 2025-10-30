//
// Created by ns on 10/29/25.
//

#ifndef GJDBCACHE_H
#define GJDBCACHE_H

#include "../../Types.h"

namespace Gj {
namespace Db {

struct Cache {
  ID id;
  ID audioFileId;

  Cache(ID id, ID audioFileId)
    : id(id)
    , audioFileId(audioFileId)
    {}

  Cache(ID audioFileId)
    : id(0)
    , audioFileId(audioFileId)
    {}
};

} // Db
} // Gj

#endif //GJDBCACHE_H
